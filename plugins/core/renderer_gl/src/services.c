#include "plugin_renderer_gl/services.h"
#include "plugin_renderer_gl/2d.h"
#include "plugin_renderer_gl/text_wrapper.h"
#include "plugin_renderer_gl/text_manager.h"
#include "plugin_renderer_gl/text.h"
#include "plugin_renderer_gl/window.h"
#include "util/services.h"

void
register_services(const struct plugin_t* plugin)
{
    SERVICE_REGISTER0(plugin, "window_width",                   window_width_wrapper, uint32_t);
    SERVICE_REGISTER0(plugin, "window_height",                  window_height_wrapper, uint32_t);
    SERVICE_REGISTER0(plugin, "shapes_2d_begin",                shapes_2d_begin_wrapper, void);
    SERVICE_REGISTER0(plugin, "shapes_2d_end",                  shapes_2d_end_wrapper, void);
    SERVICE_REGISTER5(plugin, "line_2d",                        line_2d_wrapper, void, float, float, float, float, uint32_t);
    SERVICE_REGISTER5(plugin, "box_2d",                         box_2d_wrapper, void, float, float, float, float, uint32_t);
    SERVICE_REGISTER1(plugin, "shapes_2d_destroy",              shapes_2d_destroy_wrapper, void, uint32_t);
    SERVICE_REGISTER1(plugin, "shapes_2d_show",                 shapes_show_wrapper, void, uint32_t);
    SERVICE_REGISTER1(plugin, "shapes_2d_hide",                 shapes_hide_wrapper, void, uint32_t);

    SERVICE_REGISTER2(plugin, "text_group_create",              text_group_create_wrapper, uint32_t, const char*, uint32_t);
    SERVICE_REGISTER1(plugin, "text_group_destroy",             text_group_destroy_wrapper, void, uint32_t);
    SERVICE_REGISTER2(plugin, "text_group_load_character_set",  text_group_load_character_set_wrapper, void, uint32_t, const wchar_t*);

    SERVICE_REGISTER5(plugin, "text_create",                    text_create_wrapper, uint32_t, char, uint32_t, float, float, const wchar_t*);
    SERVICE_REGISTER4(plugin, "text_destroy",                   text_destroy_wrapper, uint32_t, uint32_t, float, float, const wchar_t*);
    SERVICE_REGISTER2(plugin, "text_set_centered",              text_set_centered_wrapper, void, uint32_t, intptr_t);
    SERVICE_REGISTER1(plugin, "text_set_position",              text_set_position_wrapper, void, uint32_t);
    SERVICE_REGISTER1(plugin, "text_set_string",                text_set_string_wrapper, void, const wchar_t*);
    SERVICE_REGISTER1(plugin, "text_show",                      text_show_wrapper, void, uint32_t);
    SERVICE_REGISTER1(plugin, "text_hide",                      text_hide_wrapper, void, uint32_t);
}
