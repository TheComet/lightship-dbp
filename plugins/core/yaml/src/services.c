#include "util/services.h"
#include "plugin_yaml/services.h"
#include "plugin_yaml/parser.h"

void
register_services(const struct plugin_t* plugin)
{
    /* -----------------------------------------------------
     * All services this plugin supports
     * ---------------------------------------------------*/

    service_register(plugin, "load", (intptr_t)yaml_load);
    service_register(plugin, "get_value", (intptr_t)yaml_get_value);
    service_register(plugin, "get_dom", (intptr_t)yaml_get_dom);
    service_register(plugin, "destroy", (intptr_t)yaml_destroy);
    SERVICE_REGISTER1(plugin, "load_", yaml_load, uint32_t, const char*);
    SERVICE_REGISTER2(plugin, "get_value_", yaml_get_value, char*, uint32_t, const char*);
    SERVICE_REGISTER1(plugin, "get_dom_", yaml_get_dom, struct ptree_t*, uint32_t);
    SERVICE_REGISTER1(plugin, "destroy_", yaml_destroy, void, uint32_t);
}
