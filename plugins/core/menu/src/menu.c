#include "plugin_menu/config.h"
#include "plugin_menu/menu.h"
#include "plugin_menu/screen.h"
#include "plugin_menu/services.h"
#include "plugin_menu/button.h"
#include "plugin_menu/glob.h"
#include "framework/services.h"
#include "framework/log.h"
#include "util/ordered_vector.h"
#include "util/memory.h"
#include "util/string.h"
#include "util/yaml.h"
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
    map_init_map(&g->menu.menus);
}

/* ------------------------------------------------------------------------- */
void
menu_deinit(struct glob_t* g)
{
    while(g->menu.menus.vector.count)
    {
        struct menu_t* menu = ((struct map_key_value_t*)
            ordered_vector_back(&g->menu.menus.vector))->value;
        menu_destroy(menu);
    }

    map_clear_free(&g->menu.menus);
}

/* ------------------------------------------------------------------------- */
struct menu_t*
menu_load(struct glob_t* g, const char* file_name)
{
    struct menu_t* menu;
    struct ptree_t* doc;
    struct ptree_t* screens;
    char* menu_name;

    llog(LOG_INFO, g->game, PLUGIN_NAME, 3, "Loading menu from file \"", file_name, "\"");

    /* load and parse yaml file, get DOM */
    doc = yaml_load(file_name);
    if(!doc)
        return NULL;

    /* get ptree for screens */
    screens = yaml_get_node(doc, "screens");
    if(!screens)
    {
        llog(LOG_ERROR, g->game, PLUGIN_NAME, 1, "Failed to find \"screens\" node");
        yaml_destroy(doc);
        return NULL;
    }

    /* menu must have a name */
    {
        struct ptree_t* name_node = yaml_get_node(doc, "name");
        if(name_node && name_node->value)
            menu_name = name_node->value;
        else
        {
            llog(LOG_ERROR, g->game, PLUGIN_NAME, 1, "Failed to find \"name\" node");
            yaml_destroy(doc);
            return NULL;
        }
    }

    /* create new menu object in which to store menu elements */
    menu = (struct menu_t*)MALLOC(sizeof(struct menu_t));
    memset(menu, 0, sizeof(struct menu_t));
    map_init_map(&menu->screens);

    /* Add menu to global list */
    menu->id = ++g->menu.gid;

    /* cache glob */
    menu->glob = g;

    /* set menu name */
    menu->name = malloc_string(menu_name);

    /* load all screens into menu structure */
    menu_load_screens(menu, screens);

    /* screens are hidden by default. Show the screen specified in start_screen */
    {
        struct ptree_t* start_node = yaml_get_node(doc, "start_screen");
        if(start_node && start_node->value)
            menu_set_active_screen(menu, (char*)start_node->value);
        else
            llog(LOG_WARNING, g->game, PLUGIN_NAME, 1, "You didn't specify start_screen: \"name\" in your YAML file. Don't know which screen to begin with.");
    }

    /* clean up */
    yaml_destroy(doc);

    return menu;
}

/* ------------------------------------------------------------------------- */
void
menu_destroy(struct menu_t* menu)
{
    /* destroy all screens */
    MAP_FOR_EACH(&menu->screens, struct screen_t, key, screen)
    {
        screen_destroy(screen);
    }
    map_clear_free(&menu->screens);

    /* menu name */
    free_string(menu->name);

    /* menu object */
    FREE(menu);
}

/* ------------------------------------------------------------------------- */
void
menu_set_active_screen(struct menu_t* menu, const char* name)
{
    uint32_t screen_id = hash_jenkins_oaat(name, strlen(name));
    struct screen_t* screen = map_find(&menu->screens, screen_id);
    if(!screen)
    {
        llog(LOG_ERROR, menu->glob->game, PLUGIN_NAME, 3, "Failed to set the active screen to \"", name, "\": Screen name not found");
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
menu_load_screens(struct menu_t* menu, const struct ptree_t* screens)
{
    struct map_t created_screen_names;
    map_init_map(&created_screen_names);

    /* iterate screens */
    { YAML_FOR_EACH(screens, ".", key, screen_node)
    {
        /* handle screens */
        if(PTREE_HASH_STRING("screen") == key)
        {
            struct screen_t* screen;
            char* screen_name;

            /* get screen name */
            {
                struct ptree_t* screen_name_node = yaml_get_node(screen_node, "name");
                if(screen_name_node && screen_name_node->value)
                {
                    screen_name = (char*)screen_name_node->value;
                }
                else
                {
                    llog(LOG_WARNING, menu->glob->game, PLUGIN_NAME, 1, "Screen missing the \"name\" property. Skipping.");
                    continue;
                }
            }

            /*
            * Screen names must be unique. Verify by searching the map of
            * screens currently registered.
            */
            if(map_key_exists(&created_screen_names, hash_jenkins_oaat(screen_name, strlen(screen_name))))
            {
                llog(LOG_WARNING, menu->glob->game, PLUGIN_NAME, 2, "Screen with duplicate name found: ", screen_name);
                llog(LOG_WARNING, menu->glob->game, PLUGIN_NAME, 1, "Screen will not be created");
                continue;
            }
            map_insert(&created_screen_names, hash_jenkins_oaat(screen_name, strlen(screen_name)), NULL);

            /* create new screen object */
            screen = screen_create();
            map_insert(&menu->screens, hash_jenkins_oaat(screen_name, strlen(screen_name)), screen);

            /* iterate objects to add to this screen */
            { PTREE_FOR_EACH_IN_NODE(screen_node, key, object_node)
            {
                if(PTREE_HASH_STRING("button") == key)
                    menu_load_button(menu->glob, screen, object_node);
            }}

            /* hide the screen by default */
            screen_hide(screen);
        }
    }
    YAML_END_FOR_EACH }

    map_clear_free(&created_screen_names);
}

/* ------------------------------------------------------------------------- */
static void
menu_load_button(struct glob_t* g, struct screen_t* screen, const struct ptree_t* button_node)
{
    struct button_t* button;

    /* retrieve button parameters required to create a button */
    char* text = NULL;
    const struct ptree_t* text_node   = yaml_get_node(button_node, "text");
    const struct ptree_t* x_node      = yaml_get_node(button_node, "position.x");
    const struct ptree_t* y_node      = yaml_get_node(button_node, "position.y");
    const struct ptree_t* width_node  = yaml_get_node(button_node, "size.x");
    const struct ptree_t* height_node = yaml_get_node(button_node, "size.y");
    const struct ptree_t* action_node = yaml_get_node(button_node, "action");
    if(!x_node || !y_node || !width_node || !height_node)
    {
        llog(LOG_WARNING, g->game, PLUGIN_NAME, 1, "Not enough data to create button. Need at least position and size.");
        return;
    }
    if(text_node)
        text = (char*)text_node->value;

    /* add button to current screen */
    button = button_create(g,
                           text,  /* text is allowed to be NULL */
                           (float)atof(x_node->value),
                           (float)atof(y_node->value),
                           (float)atof(width_node->value),
                           (float)atof(height_node->value));
    screen_add_element(screen, (struct element_t*)button);

    /* extract service name and arguments tied to action, if any */
    if(action_node)
        menu_load_button_action(g, button, action_node);
}

/* ------------------------------------------------------------------------- */
static void
menu_load_button_action(struct glob_t* g, struct button_t* button, const struct ptree_t* action_node)
{
    struct ptree_t* service_node = yaml_get_node(action_node, "service");
    if(service_node && service_node->value)
    {
        struct service_t* action_service = service_get(button->base.element.glob->game, (char*)service_node->value);
        if(!action_service)
        {
            llog(LOG_WARNING, g->game, PLUGIN_NAME, 3, "Tried to bind button to service \"",
                                (char*)service_node->value,
                                "\", but the service was not found.");
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
            button->base.element.action.argv = service_create_argument_list_from_strings(action_service, &argv);
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
    struct glob_t* g = get_global(service->game);
    SERVICE_EXTRACT_ARGUMENT_PTR(0, file_name, const char*);

    struct menu_t* menu = menu_load(g, file_name);
    if(!menu)
        SERVICE_RETURN(0, uint32_t);

    SERVICE_RETURN(menu->id, uint32_t);
}

/* ------------------------------------------------------------------------- */
SERVICE(menu_destroy_wrapper)
{
    struct glob_t* g = get_global(service->game);
    SERVICE_EXTRACT_ARGUMENT_PTR(0, menu_name, const char*);

    uint32_t id = hash_jenkins_oaat(menu_name, strlen(menu_name));
    struct menu_t* menu = map_erase(&g->menu.menus, id);
    if(menu)
        menu_destroy(menu);
}

/* ------------------------------------------------------------------------- */
SERVICE(menu_set_active_screen_wrapper)
{
    struct glob_t* g = get_global(service->game);
    SERVICE_EXTRACT_ARGUMENT_PTR(0, menu_name, const char*);
    SERVICE_EXTRACT_ARGUMENT_PTR(1, screen_name, const char*);

    uint32_t menu_id = hash_jenkins_oaat(menu_name, strlen(menu_name));
    struct menu_t* menu = map_find(&g->menu.menus, menu_id);
    if(!menu)
        return;

    menu_set_active_screen(menu, screen_name);
}
