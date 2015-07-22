#include "plugin_menu/button.h"
#include "plugin_menu/services.h"
#include "plugin_menu/events.h"
#include "plugin_menu/glob.h"
#include "framework/plugin.h"
#include "framework/services.h"
#include "framework/log.h"
#include "util/map.h"
#include "util/memory.h"
#include "util/string.h"
#include <string.h>
#include <wchar.h>

#ifdef _DEBUG
static const char* ttf_filename = "../../plugins/core/menu/ttf/DejaVuSans.ttf";
#else
static const char* ttf_filename = "ttf/DejaVuSans.ttf";
#endif

static void
button_constructor(struct button_t* btn, const char* text, float x, float y, float width, float height);

static void
button_destructor(struct button_t* button);

static void
button_free_contents(struct button_t* button);

/* ------------------------------------------------------------------------- */
void button_init(struct glob_t* g)
{
	uint32_t char_size;

	/* initialise container in which all buttons are stored */
	map_init_map(&g->button.buttons);

	/* load font and characters */
	char_size = 9;
	SERVICE_CALL2(g->services.text_group_create, &g->button.font_id, PTR(ttf_filename), char_size);
	SERVICE_CALL2(g->services.text_group_load_character_set, NULL, g->button.font_id, PTR(NULL));
}

/* ------------------------------------------------------------------------- */
void button_deinit(struct glob_t* g)
{
	SERVICE_CALL1(g->services.text_group_destroy, NULL, g->button.font_id);
	button_destroy_all(g);
	map_clear_free(&g->button.buttons);
}

/* ------------------------------------------------------------------------- */
struct button_t*
button_create(struct glob_t* g, const char* text, float x, float y, float width, float height)
{
	struct button_t* btn = (struct button_t*)MALLOC(sizeof(struct button_t));
	memset(btn, 0, sizeof(struct button_t));

	/* base constructor */
	element_constructor(g,
						(struct element_t*)btn,
						(element_destructor_func)button_destructor,
						x, y,
						width, height);

	/* derived constructor */
	button_constructor(btn, text, x, y, width, height);

	return btn;
}

/* ------------------------------------------------------------------------- */
static void
button_constructor(struct button_t* btn, const char* text, float x, float y, float width, float height)
{
	/* base struct is constructed first, so we can safely get the glob struct */
	struct glob_t* g = btn->base.element.glob;

	/* copy wchar_t string into button object */
	if(text)
	{
		char is_centered = 1;
		float offy = y + 0.02f;
		/* TODO centering code for text */
		/* TODO instead of passing the raw string, add way to pass a "string instance"
		* which can specify the font and size of the string. */
		btn->base.button.text = strtowcs(text);
		SERVICE_CALL5(g->services.text_create, &btn->base.button.text_id, g->button.font_id, is_centered, x, offy, PTR(btn->base.button.text));
		element_add_text((struct element_t*)btn, g->button.font_id, btn->base.button.text_id);
	}
	else
	{
		btn->base.button.text = NULL;
		btn->base.button.text_id = 0;
	}

	/* draw box */
	SERVICE_CALL0(g->services.shapes_2d_begin, NULL);
	{
		float x1, y1, x2, y2;
		uint32_t colour = BUTTON_COLOUR_NORMAL;
		x1 = x - width  * 0.5f;
		y1 = y - height * 0.5f;
		x2 = x + width  * 0.5f;
		y2 = y + height * 0.5f;
		SERVICE_CALL5(g->services.box_2d, NULL, x1, y1, x2, y2, colour);
	}
	SERVICE_CALL0(g->services.shapes_2d_end, &btn->base.button.shapes_normal_id);
	element_add_shapes((struct element_t*)btn, btn->base.button.shapes_normal_id);

	/* add to global list of buttons */
	map_insert(&g->button.buttons, btn->base.element.id, btn);
}

/* ------------------------------------------------------------------------- */
static void
button_destructor(struct button_t* button)
{
	struct glob_t* g = button->base.element.glob;
	map_erase(&g->button.buttons, button->base.element.id);
	button_free_contents(button);
}

/* ------------------------------------------------------------------------- */
void
button_destroy(struct button_t* button)
{
	button_destructor(button);
	element_destructor((struct element_t*)button);
	FREE(button);
}

/* ------------------------------------------------------------------------- */
void
button_destroy_all(struct glob_t* g)
{
	struct button_t* button;
	while((button = map_get_any(&g->button.buttons)))
	{
		button_destroy(button);
	}
}

/* ------------------------------------------------------------------------- */
void
button_free_contents(struct button_t* button)
{
	struct glob_t* g = button->base.element.glob;

	if(button->base.button.text)
	{
		SERVICE_CALL1(g->services.text_destroy, NULL, button->base.button.text_id);
		free_string(button->base.button.text);
		if(button->base.element.action.service)
			dynamic_call_destroy_argument_vector(button->base.element.action.service->type_info,
												 button->base.element.action.argv);
	}
}

/* ------------------------------------------------------------------------- */
struct button_t*
button_collision(struct glob_t* g, struct button_t* button, float x, float y)
{
	/* test specified button */
	if(button && button->base.element.visible)
	{
		struct element_data_t* elem;
		if(!button->base.element.visible)
			return NULL;

		elem = &button->base.element;
		if(x > elem->pos.x - elem->size.x*0.5 && x < elem->pos.x + elem->size.x*0.5)
			if(y > elem->pos.y - elem->size.y*0.5 && y < elem->pos.y + elem->size.y*0.5)
				return button;
		return NULL;
	}

	/* test all buttons */
	{ MAP_FOR_EACH(&g->button.buttons, struct button_t, id, cur_btn)
	{
		struct element_data_t* elem;
		if(!cur_btn->base.element.visible)
			continue;

		elem = &cur_btn->base.element;
		if(x > elem->pos.x - elem->size.x*0.5 && x < elem->pos.x + elem->size.x*0.5)
			if(y > elem->pos.y - elem->size.y*0.5 && y < elem->pos.y + elem->size.y*0.5)
				return cur_btn;
	}}
	return NULL;
}

/* ------------------------------------------------------------------------- */
EVENT_LISTENER(on_mouse_clicked)
{
	/* EXTRACT_ARGUMENT(0, mouse_btn, char, char); */
	EXTRACT_ARGUMENT(1, x, double, double);
	EXTRACT_ARGUMENT(2, y, double, double);

	struct glob_t* g = get_global(event->plugin->game);
	struct button_t* button = button_collision(g, NULL, (float)x, (float)y);

	if(button)
	{
		/* let everything know it was clicked */
		EVENT_FIRE1(g->events.button_clicked, button->base.element.id);

		/* if button has an action, execute it */
		if(button->base.element.action.service)
		{
			/* Pass vector of args (if there are no args, argv->data should be NULL */
			/* Ignore the return value */
			button->base.element.action.service->exec(button->base.element.action.service,
													  NULL,
													  (const void**)button->base.element.action.argv);
		}
	}
}

/* ----------------------------------------------------------------------------
 * WRAPPERS
 * --------------------------------------------------------------------------*/

SERVICE(button_create_wrapper)
{
	struct glob_t* g = get_global(service->plugin->game);
	EXTRACT_ARGUMENT(0, text, const char*, const char*);
	EXTRACT_ARGUMENT(1, x, float, float);
	EXTRACT_ARGUMENT(2, y, float, float);
	EXTRACT_ARGUMENT(3, width, float, float);
	EXTRACT_ARGUMENT(4, height, float, float);
	RETURN(button_create(g, text, x, y, width, height), uintptr_t);
}

/* ------------------------------------------------------------------------- */
SERVICE(button_destroy_wrapper)
{
	EXTRACT_ARGUMENT(0, button, uintptr_t, struct button_t*);
	button_destroy(button);
}

/* ------------------------------------------------------------------------- */
SERVICE(button_get_text_wrapper)
{
	EXTRACT_ARGUMENT(0, button, uintptr_t, struct button_t*);
	RETURN(button->base.button.text, wchar_t*);
}
