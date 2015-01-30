#include "plugin_renderer_gl/services.h"
#include "plugin_renderer_gl/2d.h"
#include "plugin_renderer_gl/text.h"
#include "plugin_renderer_gl/window.h"
#include "util/services.h"

void
register_services(const struct plugin_t* plugin)
{
    SERVICE_REGISTER0(plugin, "window_width",                   window_width, uint32_t);
    SERVICE_REGISTER0(plugin, "window_height",                  window_height, uint32_t);
    SERVICE_REGISTER0(plugin, "shapes_2d_begin",                shapes_2d_begin, void);
    SERVICE_REGISTER0(plugin, "shapes_2d_end",                  shapes_2d_end, void);
    SERVICE_REGISTER5(plugin, "line_2d",                        line_2d, void, float, float, float, float, uint32_t);
    SERVICE_REGISTER5(plugin, "box_2d",                         box_2d, void, float, float, float, float, uint32_t);
    SERVICE_REGISTER1(plugin, "shapes_2d_destroy",              shapes_2d_destroy, void, uint32_t);
    SERVICE_REGISTER2(plugin, "text_load_font",                 text_load_font_wrapper, uint32_t, const char*, uint32_t);
    SERVICE_REGISTER1(plugin, "text_destroy_font",              text_destroy_font_wrapper, void, uint32_t);
    SERVICE_REGISTER2(plugin, "text_load_characters",           text_load_characters_wrapper, void, uint32_t, const wchar_t*);
    SERVICE_REGISTER4(plugin, "text_add_static_string",         text_add_static_string_wrapper, intptr_t, uint32_t, float, float, const wchar_t*);
    SERVICE_REGISTER4(plugin, "text_add_static_center_string",  text_add_static_center_string_wrapper, intptr_t, uint32_t, float, float, const wchar_t*);
    SERVICE_REGISTER2(plugin, "text_destroy_static_string",     text_destroy_static_string_wrapper, void, uint32_t, intptr_t);
    SERVICE_REGISTER1(plugin, "text_destroy_all_static_strings",text_destroy_all_static_strings_wrapper, void, uint32_t);
}
