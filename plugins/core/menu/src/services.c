#include "util/services.h"
#include "plugin_menu/services.h"
#include "plugin_menu/button.h"
#include "plugin_menu/screen_manager.h"

/* extern service function pointers */
yaml_load_func yaml_load;
yaml_get_value_func yaml_get_value;
yaml_get_dom_func yaml_get_dom;
yaml_destroy_func yaml_destroy;

shapes_2d_begin_func shapes_2d_begin;
shapes_2d_end_func shapes_2d_end;
shapes_2d_destroy_func shapes_2d_destroy;
line_2d_func line_2d;
box_2d_func box_2d;

text_load_font_func text_load_font;
text_destroy_font_func text_destroy_font;
text_load_characters_func text_load_characters;
text_add_static_center_string_func text_add_static_center_string;
text_destroy_static_string_func text_destroy_static_string;

void
register_services(const struct plugin_t* plugin)
{
    /* -----------------------------------------------------
    * All services this plugin supports
    * ---------------------------------------------------*/

     service_register(plugin, "button_create", (intptr_t)button_create_wrapper);
     service_register(plugin, "button_destroy", (intptr_t)button_destroy_wrapper);
     service_register(plugin, "button_get_text", (intptr_t)button_get_text);
     service_register(plugin, "load", (intptr_t)menu_load);
}

char
get_required_services(void)
{
    if(!(yaml_load = (yaml_load_func)service_get("yaml.load")))
        return 0;
    if(!(yaml_get_value = (yaml_get_value_func)service_get("yaml.get_value")))
        return 0;
    if(!(yaml_get_dom = (yaml_get_dom_func)service_get("yaml.get_dom")))
        return 0;
    if(!(yaml_destroy = (yaml_destroy_func)service_get("yaml.destroy")))
        return 0;
    if(!(shapes_2d_begin = (shapes_2d_begin_func)service_get("renderer_gl.shapes_2d_begin")))
        return 0;
    if(!(shapes_2d_end = (shapes_2d_end_func)service_get("renderer_gl.shapes_2d_end")))
        return 0;
    if(!(shapes_2d_destroy = (shapes_2d_destroy_func)service_get("renderer_gl.shapes_2d_destroy")))
        return 0;
    if(!(line_2d = (line_2d_func)service_get("renderer_gl.line_2d")))
        return 0;
    if(!(box_2d = (box_2d_func)service_get("renderer_gl.box_2d")))
        return 0;
    if(!(text_load_font = (text_load_font_func)service_get("renderer_gl.text_load_font")))
        return 0;
    if(!(text_destroy_font = (text_destroy_font_func)service_get("renderer_gl.text_destroy_font")))
        return 0;
    if(!(text_load_characters = (text_load_characters_func)service_get("renderer_gl.text_load_characters")))
        return 0;
    if(!(text_add_static_center_string = (text_add_static_center_string_func)service_get("renderer_gl.text_add_static_center_string")))
        return 0;
    if(!(text_destroy_static_string = (text_destroy_static_string_func)service_get("renderer_gl.text_destroy_static_string")))
        return 0;
    return 1;
}

void get_optional_services(void)
{
}
