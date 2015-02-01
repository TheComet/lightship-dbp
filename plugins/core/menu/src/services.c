#include "util/services.h"
#include "plugin_menu/services.h"
#include "plugin_menu/button.h"
#include "plugin_menu/menu.h"

/* external services used by this plugin */
struct service_t* yaml_load;
struct service_t* yaml_get_value;
struct service_t* yaml_get_dom;
struct service_t* yaml_destroy;

struct service_t* shapes_2d_begin;
struct service_t* shapes_2d_end;
struct service_t* shapes_2d_destroy;
struct service_t* line_2d;
struct service_t* box_2d;

struct service_t* text_load_font;
struct service_t* text_destroy_font;
struct service_t* text_load_characters;
struct service_t* text_add_static_center_string;
struct service_t* text_destroy_static_string;

SERVICE(foo)
{
    SERVICE_EXTRACT_ARGUMENT_PTR(0, t1, const char*);
    SERVICE_EXTRACT_ARGUMENT_PTR(1, t2, const char*);
    SERVICE_EXTRACT_ARGUMENT_PTR(2, t3, const char*);
    printf("received strings: %s, %s, %s\n", t1, t2, t3);
}

void
register_services(const struct plugin_t* plugin)
{
    /* -----------------------------------------------------
     * All services this plugin supports
     * ---------------------------------------------------*/

    SERVICE_REGISTER5(plugin, "button_create", button_create_wrapper, intptr_t, const char*, float, float, float, float);
    SERVICE_REGISTER1(plugin, "button_destroy", button_destroy_wrapper, void, intptr_t);
    SERVICE_REGISTER1(plugin, "button_get_text", button_get_text_wrapper, wchar_t*, intptr_t);
    SERVICE_REGISTER1(plugin, "load", menu_load_wrapper, struct menu_t, const char*);
    SERVICE_REGISTER3(plugin, "foo", foo, void, const char*, const char*, const char*);
}

char
get_required_services(void)
{
    if(!(yaml_load                      = service_get("yaml.load")))
        return 0;
    if(!(yaml_get_value                 = service_get("yaml.get_value")))
        return 0;
    if(!(yaml_get_dom                   = service_get("yaml.get_dom")))
        return 0;
    if(!(yaml_destroy                   = service_get("yaml.destroy")))
        return 0;
    if(!(shapes_2d_begin                = service_get("renderer_gl.shapes_2d_begin")))
        return 0;
    if(!(shapes_2d_end                  = service_get("renderer_gl.shapes_2d_end")))
        return 0;
    if(!(shapes_2d_destroy              = service_get("renderer_gl.shapes_2d_destroy")))
        return 0;
    if(!(line_2d                        = service_get("renderer_gl.line_2d")))
        return 0;
    if(!(box_2d                         = service_get("renderer_gl.box_2d")))
        return 0;
    if(!(text_load_font                 = service_get("renderer_gl.text_load_font")))
        return 0;
    if(!(text_destroy_font              = service_get("renderer_gl.text_destroy_font")))
        return 0;
    if(!(text_load_characters           = service_get("renderer_gl.text_load_characters")))
        return 0;
    if(!(text_add_static_center_string  = service_get("renderer_gl.text_add_static_center_string")))
        return 0;
    if(!(text_destroy_static_string     = service_get("renderer_gl.text_destroy_static_string")))
        return 0;
    return 1;
}

void get_optional_services(void)
{
}
