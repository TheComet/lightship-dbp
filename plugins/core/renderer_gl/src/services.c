#include "plugin_renderer_gl/services.h"
#include "plugin_renderer_gl/2d.h"
#include "plugin_renderer_gl/text.h"
#include "util/services.h"

void
register_services(const struct plugin_t* plugin)
{
    service_register(plugin, "shapes_2d_begin",                 (intptr_t)shapes_2d_begin);
    service_register(plugin, "shapes_2d_end",                   (intptr_t)shapes_2d_end);
    service_register(plugin, "line_2d",                         (intptr_t)line_2d);
    service_register(plugin, "box_2d",                          (intptr_t)box_2d);
    service_register(plugin, "shapes_2d_destroy",               (intptr_t)shapes_2d_destroy);
    service_register(plugin, "text_load_font",                  (intptr_t)text_load_font_wrapper);
    service_register(plugin, "text_destroy_font",               (intptr_t)text_destroy_font_wrapper);
    service_register(plugin, "text_load_characters",            (intptr_t)text_load_characters_wrapper);
    service_register(plugin, "text_add_static_string",          (intptr_t)text_add_static_string_wrapper);
    service_register(plugin, "text_destroy_static_string",      (intptr_t)text_destroy_static_string_wrapper);
    service_register(plugin, "text_destroy_all_static_strings", (intptr_t)text_destroy_all_static_strings_wrapper);
}
