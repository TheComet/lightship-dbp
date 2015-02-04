#include "util/services.h"
#include "plugin_menu/services.h"
#include "plugin_menu/button.h"
#include "plugin_menu/menu.h"

/* external services used by this plugin */
struct service_t* yaml_load = NULL;
struct service_t* yaml_get_value = NULL;
struct service_t* yaml_get_dom = NULL;
struct service_t* yaml_destroy = NULL;

struct service_t* shapes_2d_begin = NULL;
struct service_t* shapes_2d_end = NULL;
struct service_t* shapes_2d_destroy = NULL;
struct service_t* line_2d = NULL;
struct service_t* box_2d = NULL;
struct service_t* shapes_2d_show = NULL;
struct service_t* shapes_2d_hide = NULL;

struct service_t* text_group_create = NULL;
struct service_t* text_group_destroy = NULL;
struct service_t* text_group_load_character_set = NULL;

struct service_t* text_create = NULL;
struct service_t* text_destroy = NULL;
struct service_t* text_show = NULL;
struct service_t* text_hide = NULL;

void
register_services(const struct plugin_t* plugin)
{
    /* -----------------------------------------------------
     * All services this plugin supports
     * ---------------------------------------------------*/

    SERVICE_REGISTER5(plugin, "button_create", button_create_wrapper, intptr_t, const char*, float, float, float, float);
    SERVICE_REGISTER1(plugin, "button_destroy", button_destroy_wrapper, void, intptr_t);
    SERVICE_REGISTER1(plugin, "button_get_text", button_get_text_wrapper, wchar_t*, intptr_t);
    SERVICE_REGISTER1(plugin, "load", menu_load_wrapper, const char*, const char*);
    SERVICE_REGISTER1(plugin, "destroy", menu_destroy_wrapper, void, const char*);
    SERVICE_REGISTER2(plugin, "set_active_screen", menu_set_active_screen_wrapper, void, const char*, const char*);
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
    if(!(shapes_2d_show                 = service_get("renderer_gl.shapes_2d_show")))
        return 0;
    if(!(shapes_2d_hide                 = service_get("renderer_gl.shapes_2d_hide")))
        return 0;
    
    if(!(text_group_create              = service_get("renderer_gl.text_group_create")))
        return 0;
    if(!(text_group_destroy             = service_get("renderer_gl.text_group_destroy")))
        return 0;
    if(!(text_group_load_character_set  = service_get("renderer_gl.text_group_load_character_set")))
        return 0;
    if(!(text_create                    = service_get("renderer_gl.text_create")))
        return 0;
    if(!(text_destroy                   = service_get("renderer_gl.text_destroy")))
        return 0;
    if(!(text_show                      = service_get("renderer_gl.text_show")))
        return 0;
    if(!(text_hide                      = service_get("renderer_gl.text_hide")))
        return 0;

    return 1;
}

void get_optional_services(void)
{
}
