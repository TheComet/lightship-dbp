#include "util/services.h"
#include "plugin_menu/services.h"

/* extern service function pointers */
yaml_load_func yaml_load;
yaml_get_value_func yaml_get_value;
yaml_get_dom_func yaml_get_dom;
yaml_destroy_func yaml_destroy;

void
register_services(const struct plugin_t* plugin)
{
    /* -----------------------------------------------------
    * All services this plugin supports
    * ---------------------------------------------------*/
    /*
     * service_register(plugin, "service_name_1", service_callback_1);
     * service_register(plugin, "service_name_2", service_callback_2);
     * etc...
     */

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
    return 1;
}

void get_optional_services(void)
{
}
