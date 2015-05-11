#include "plugin_renderer_gl/config.h"
#include "plugin_renderer_gl/services.h"
#include "plugin_renderer_gl/2d.h"
#include "plugin_renderer_gl/sprite.h"
#include "plugin_renderer_gl/text_wrapper.h"
#include "plugin_renderer_gl/text_manager.h"
#include "plugin_renderer_gl/text.h"
#include "plugin_renderer_gl/window.h"
#include "framework/services.h"

void
register_services(struct game_t* game, struct plugin_t* plugin)
{
    struct service_t* s;
    SERVICE_CREATE0(game, s, PLUGIN_NAME ".window_width",                   window_width_wrapper, uint32_t);
    SERVICE_CREATE0(game, s, PLUGIN_NAME ".window_height",                  window_height_wrapper, uint32_t);
    SERVICE_CREATE0(game, s, PLUGIN_NAME ".shapes_2d_begin",                shapes_2d_begin_wrapper, void);
    SERVICE_CREATE0(game, s, PLUGIN_NAME ".shapes_2d_end",                  shapes_2d_end_wrapper, void);
    SERVICE_CREATE5(game, s, PLUGIN_NAME ".line_2d",                        line_2d_wrapper, void, float, float, float, float, uint32_t);
    SERVICE_CREATE5(game, s, PLUGIN_NAME ".box_2d",                         box_2d_wrapper, void, float, float, float, float, uint32_t);
    SERVICE_CREATE1(game, s, PLUGIN_NAME ".shapes_2d_destroy",              shapes_2d_destroy_wrapper, void, uint32_t);
    SERVICE_CREATE1(game, s, PLUGIN_NAME ".shapes_2d_show",                 shapes_show_wrapper, void, uint32_t);
    SERVICE_CREATE1(game, s, PLUGIN_NAME ".shapes_2d_hide",                 shapes_hide_wrapper, void, uint32_t);

    SERVICE_CREATE4(game, s, PLUGIN_NAME ".sprite_create",                  sprite_create_wrapper, uint32_t, char*, uint16_t, uint16_t, uint16_t);
    SERVICE_CREATE6(game, s, PLUGIN_NAME ".sprite_create_from_memory",      sprite_create_from_memory_wrapper, uint32_t, unsigned char*, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
    SERVICE_CREATE1(game, s, PLUGIN_NAME ".sprite_destroy",                 sprite_destroy_wrapper, void, uint32_t);

    SERVICE_CREATE2(game, s, PLUGIN_NAME ".text_group_create",              text_group_create_wrapper, uint32_t, const char*, uint32_t);
    SERVICE_CREATE1(game, s, PLUGIN_NAME ".text_group_destroy",             text_group_destroy_wrapper, void, uint32_t);
    SERVICE_CREATE2(game, s, PLUGIN_NAME ".text_group_load_character_set",  text_group_load_character_set_wrapper, void, uint32_t, const wchar_t*);

    SERVICE_CREATE5(game, s, PLUGIN_NAME ".text_create",                    text_create_wrapper, uint32_t, char, uint32_t, float, float, const wchar_t*);
    SERVICE_CREATE4(game, s, PLUGIN_NAME ".text_destroy",                   text_destroy_wrapper, uint32_t, uint32_t, float, float, const wchar_t*);
    SERVICE_CREATE2(game, s, PLUGIN_NAME ".text_set_centered",              text_set_centered_wrapper, void, uint32_t, char);
    SERVICE_CREATE3(game, s, PLUGIN_NAME ".text_set_position",              text_set_position_wrapper, void, uint32_t, float, float);
    SERVICE_CREATE1(game, s, PLUGIN_NAME ".text_set_string",                text_set_string_wrapper, void, const wchar_t*);
    SERVICE_CREATE1(game, s, PLUGIN_NAME ".text_show",                      text_show_wrapper, void, uint32_t);
    SERVICE_CREATE1(game, s, PLUGIN_NAME ".text_hide",                      text_hide_wrapper, void, uint32_t);
}
