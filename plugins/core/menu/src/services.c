#include "framework/services.h"
#include "framework/plugin.h"
#include "plugin_menu/config.h"
#include "plugin_menu/services.h"
#include "plugin_menu/button.h"
#include "plugin_menu/context.h"
#include "plugin_menu/menu.h"
#include <string.h>

/* ------------------------------------------------------------------------- */
void
register_services(struct plugin_t* plugin)
{
	/* -----------------------------------------------------
	 * All services this plugin supports
	 * ---------------------------------------------------*/

	struct service_t* s;
	SERVICE_CREATE5(plugin, s, PLUGIN_NAME ".button_create",        button_create_wrapper, uintptr_t, const char*, float, float, float, float);
	SERVICE_CREATE1(plugin, s, PLUGIN_NAME ".button_destroy",       button_destroy_wrapper, void, uintptr_t);
	SERVICE_CREATE1(plugin, s, PLUGIN_NAME ".button_get_text",      button_get_text_wrapper, wchar_t*, uintptr_t);
	SERVICE_CREATE1(plugin, s, PLUGIN_NAME ".load",                 menu_load_wrapper, const char*, const char*);
	SERVICE_CREATE1(plugin, s, PLUGIN_NAME ".destroy",              menu_destroy_wrapper, void, const char*);
	SERVICE_CREATE2(plugin, s, PLUGIN_NAME ".set_active_screen",    menu_set_active_screen_wrapper, void, const char*, const char*);
}

/* ------------------------------------------------------------------------- */
char
get_required_services(struct plugin_t* plugin)
{
	struct game_t* game = plugin->game;

	/* get service context and set every service pointer to NULL */
	struct context_services_t* g = &get_context(game)->services;
	memset(g, 0, sizeof(struct context_services_t));

	if(!(g->shapes_2d_begin                = service_get(game, "renderer_gl.shapes_2d_begin")))
		return 0;
	if(!(g->shapes_2d_end                  = service_get(game, "renderer_gl.shapes_2d_end")))
		return 0;
	if(!(g->shapes_2d_destroy              = service_get(game, "renderer_gl.shapes_2d_destroy")))
		return 0;
	if(!(g->line_2d                        = service_get(game, "renderer_gl.line_2d")))
		return 0;
	if(!(g->box_2d                         = service_get(game, "renderer_gl.box_2d")))
		return 0;
	if(!(g->shapes_2d_show                 = service_get(game, "renderer_gl.shapes_2d_show")))
		return 0;
	if(!(g->shapes_2d_hide                 = service_get(game, "renderer_gl.shapes_2d_hide")))
		return 0;

	if(!(g->text_group_create              = service_get(game, "renderer_gl.text_group_create")))
		return 0;
	if(!(g->text_group_destroy             = service_get(game, "renderer_gl.text_group_destroy")))
		return 0;
	if(!(g->text_group_load_character_set  = service_get(game, "renderer_gl.text_group_load_character_set")))
		return 0;
	if(!(g->text_create                    = service_get(game, "renderer_gl.text_create")))
		return 0;
	if(!(g->text_destroy                   = service_get(game, "renderer_gl.text_destroy")))
		return 0;
	if(!(g->text_show                      = service_get(game, "renderer_gl.text_show")))
		return 0;
	if(!(g->text_hide                      = service_get(game, "renderer_gl.text_hide")))
		return 0;

	return 1;
}

/* ------------------------------------------------------------------------- */
void
get_optional_services(struct plugin_t* plugin)
{
}
