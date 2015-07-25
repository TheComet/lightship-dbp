#include "plugin_menu/config.h"
#include "plugin_menu/menu.h"
#include "plugin_menu/screen.h"
#include "plugin_menu/services.h"
#include "plugin_menu/button.h"
#include "plugin_menu/glob.h"
#include "framework/plugin.h"
#include "framework/services.h"
#include "framework/log.h"
#include "util/ordered_vector.h"
#include "util/memory.h"
#include "util/string.h"
#include "util/yaml.h"
#include "util/dynamic_call.h"
#include <string.h>
#include <stdlib.h>

static void
menu_load_screens(struct menu_t* menu, const struct ptree_t* screen_node);

static void
menu_load_button(struct glob_t* g, struct screen_t* screen, const struct ptree_t* button_node);

static void
menu_load_button_action(struct glob_t* g, struct button_t* button, const struct ptree_t* action_node);

/* ------------------------------------------------------------------------- */
void
menu_init(struct glob_t* g)
{
	bsthv_init_bsthv(&g->menu.menus);
}

/* ------------------------------------------------------------------------- */
void
menu_deinit(struct glob_t* g)
{
	struct menu_t* menu;
	while((menu = bsthv_get_any_element(&g->menu.menus)))
	{
		menu_destroy(menu);
	}
	bsthv_clear_free(&g->menu.menus);
}

/* ------------------------------------------------------------------------- */
struct menu_t*
menu_load(struct glob_t* g, const char* file_name)
{
	struct menu_t* menu;
	struct ptree_t* doc;
	const char* menu_name;

	llog(LOG_INFO, g->game, PLUGIN_NAME, "Loading menu from file \"%s\"", file_name);

	/* load and parse yaml file, get DOM */
	doc = yaml_load(file_name);
	if(!doc)
		return NULL;

	/* get ptree for screens */
	if(!yaml_get_node(doc, "screens"))
	{
		llog(LOG_ERROR, g->game, PLUGIN_NAME, "Failed to find \"screens\" node");
		yaml_destroy(doc);
		return NULL;
	}

	/* menu must have a name */
	menu_name = yaml_get_value(doc, "name");
	if(!menu_name)
	{
		llog(LOG_ERROR, g->game, PLUGIN_NAME, "Failed to find \"name\" node");
		yaml_destroy(doc);
		return NULL;
	}

	/* create new menu object in which to store menu elements */
	menu = (struct menu_t*)MALLOC(sizeof(struct menu_t));
	memset(menu, 0, sizeof(struct menu_t));
	bsthv_init_bsthv(&menu->screens);

	/* set the name of the menu */
	menu->name = malloc_string(menu_name);

	/* cache glob */
	menu->glob = g;

	/* load all screens into menu structure */
	menu_load_screens(menu, doc);

	/* screens are hidden by default. Show the screen specified in start_screen */
	{
		const char* start_screen;
		if((start_screen = yaml_get_value(doc, "start_screen")))
			menu_set_active_screen(menu, start_screen);
		else
			llog(LOG_WARNING, g->game, PLUGIN_NAME, "You didn't specify start_screen: \"name\" in your YAML file. Don't know which screen to begin with.");
	}

	/* insert into global list of menus */
	bsthv_insert(&g->menu.menus, menu->name, menu);

	/* clean up */
	yaml_destroy(doc);

	return menu;
}

/* ------------------------------------------------------------------------- */
void
menu_destroy(struct menu_t* menu)
{
	/* destroy all screens */
	BSTHV_FOR_EACH(&menu->screens, struct screen_t, key, screen)
		screen_destroy(screen);
	BSTHV_END_EACH
	bsthv_clear_free(&menu->screens);

	/* remove from global list of menus */
	bsthv_erase(&menu->glob->menu.menus, menu->name);

	/* menu name */
	free_string(menu->name);

	/* menu object */
	FREE(menu);
}

/* ------------------------------------------------------------------------- */
void
menu_set_active_screen(struct menu_t* menu, const char* screen_name)
{
	struct screen_t* screen = bsthv_find(&menu->screens, screen_name);
	if(!screen)
	{
		llog(LOG_ERROR, menu->glob->game, PLUGIN_NAME, "Failed to set the "
			"active screen to \"%s\": Screen name not found", screen_name);
		return;
	}

	if(menu->active_screen)
		screen_hide(menu->active_screen);

	screen_show(screen);
	menu->active_screen = screen;
}

/* ------------------------------------------------------------------------- */
/* STATIC FUNCTIONS */
/* ------------------------------------------------------------------------- */
static void
menu_load_screens(struct menu_t* menu, const struct ptree_t* doc)
{
	struct bsthv_t created_screen_names;
	bsthv_init_bsthv(&created_screen_names);

	/* iterate screens */
	YAML_FOR_EACH(doc, "screens", key, screen_node)
		struct screen_t* screen;
		const char* screen_name = yaml_get_value(screen_node, "name");

		/* get screen name */
		if(!screen_name)
		{
			llog(LOG_WARNING, menu->glob->game, PLUGIN_NAME, "Screen missing the \"name\" property. Skipping.");
			continue;
		}

		/*
		* Screen names must be unique. Verify by searching the map of
		* screens currently registered.
		*/
		if(bsthv_key_exists(&created_screen_names, screen_name))
		{
			llog(LOG_WARNING, menu->glob->game, PLUGIN_NAME, "Screen with duplicate name found: %s", screen_name);
			llog(LOG_WARNING, menu->glob->game, PLUGIN_NAME, "Screen will not be created");
			continue;
		}
		bsthv_insert(&created_screen_names, screen_name, NULL);

		/* create new screen object */
		screen = screen_create();
		bsthv_insert(&menu->screens, screen_name, screen);

		/* iterate objects to add to this screen */
		YAML_FOR_EACH(screen_node, "buttons", key, button_node)
			menu_load_button(menu->glob, screen, button_node);
		YAML_END_EACH

		/* hide the screen by default */
		screen_hide(screen);
	YAML_END_EACH

	bsthv_clear_free(&created_screen_names);
}

/* ------------------------------------------------------------------------- */
static void
menu_load_button(struct glob_t* g, struct screen_t* screen, const struct ptree_t* button_node)
{
	struct button_t* button;

	/* retrieve button parameters required to create a button */
	const char* text = yaml_get_value(button_node, "text");
	const char* x_str      = yaml_get_value(button_node, "position.x");
	const char* y_str      = yaml_get_value(button_node, "position.y");
	const char* width_str  = yaml_get_value(button_node, "size.x");
	const char* height_str = yaml_get_value(button_node, "size.y");
	const struct ptree_t* action_node = yaml_get_node(button_node, "action");
	if(!x_str || !y_str || !width_str || !height_str)
	{
		llog(LOG_WARNING, g->game, PLUGIN_NAME, "Not enough data to create "
			"button. Need at least position and size.");
		return;
	}

	/* add button to current screen */
	button = button_create(g,
						   text,  /* text is allowed to be NULL */
						   (float)atof(x_str),
						   (float)atof(y_str),
						   (float)atof(width_str),
						   (float)atof(height_str));
	screen_add_element(screen, (struct element_t*)button);

	/* extract service name and arguments tied to action, if any */
	if(action_node)
		menu_load_button_action(g, button, action_node);
}

/* ------------------------------------------------------------------------- */
static void
menu_load_button_action(struct glob_t* g, struct button_t* button, const struct ptree_t* action_node)
{
	const char* service_str = yaml_get_value(action_node, "service");
	if(service_str)
	{
		struct service_t* action_service;
		if(!(action_service = service_get(button->base.element.glob->game, service_str)))
		{
			llog(LOG_WARNING, g->game, PLUGIN_NAME, "Tried to bind button "
				"to service \"%s\", but the service was not found.", service_str);
		}
		else
		{
			/*
			 * The service exists and can be called. Extract
			 * arguments and create compatible argument list.
			 */
			struct ptree_t* argv_node;
			struct ordered_vector_t argv;
			char arg_key[sizeof(int)*8+1];
			int action_argc = 0;
			ordered_vector_init_vector(&argv, sizeof(char*));

			/* extract each argument and insert into vector as string */
			argv_node = yaml_get_node(action_node, "argv");
			while(argv_node)
			{
				/* retrieve next argument */
				struct ptree_t* arg_node;
				sprintf(arg_key, "%d", action_argc);
				arg_node = yaml_get_node(argv_node, arg_key);
				if(!arg_node)
					break;
				/* argument found, add to argument list */
				ordered_vector_push(&argv, &arg_node->value);
				++action_argc;
			}

			/* convert the vector of strings into a vector of arguments */
			button->base.element.action.argv =
					dynamic_call_create_argument_vector_from_strings(action_service->type_info, &argv);
			ordered_vector_clear_free(&argv);
			/* only set service if the arguments were successfully created */
			if(button->base.element.action.argv)
			{
				button->base.element.action.service = action_service;
			}
		}

	}
}

/* ------------------------------------------------------------------------- */
/* WRAPPERS */
/* ------------------------------------------------------------------------- */
SERVICE(menu_load_wrapper)
{
	struct glob_t* g = get_global(service->plugin->game);
	EXTRACT_ARGUMENT_PTR(0, file_name, const char*);

	struct menu_t* menu = menu_load(g, file_name);
	if(!menu)
		RETURN(NULL, const char*);
	RETURN(menu->name, const char*);
}

/* ------------------------------------------------------------------------- */
SERVICE(menu_destroy_wrapper)
{
	struct glob_t* g = get_global(service->plugin->game);
	EXTRACT_ARGUMENT_PTR(0, menu_name, const char*);

	struct menu_t* menu = bsthv_find(&g->menu.menus, menu_name);
	if(menu)
		menu_destroy(menu);
}

/* ------------------------------------------------------------------------- */
SERVICE(menu_set_active_screen_wrapper)
{
	struct glob_t* g = get_global(service->plugin->game);
	EXTRACT_ARGUMENT_PTR(0, menu_name, const char*);
	EXTRACT_ARGUMENT_PTR(1, screen_name, const char*);

	struct menu_t* menu = bsthv_find(&g->menu.menus, menu_name);
	if(menu)
	{
		menu_set_active_screen(menu, screen_name);
		return;
	}

	llog(LOG_WARNING, service->plugin->game, PLUGIN_NAME, "Failed to "
		"set the active screen to \"%s\" in menu \"%s\" - menu was not found.",
		 screen_name, menu_name
	);
}
