#include "util/services.h"
#include "plugin_yaml/services.h"
#include "plugin_yaml/parser.h"

void register_services(struct plugin_t* plugin)
{
    /* -----------------------------------------------------
    * All services this plugin supports
    * ---------------------------------------------------*/

    service_register(plugin, "load", (intptr_t)yaml_load);
    service_register(plugin, "get", (intptr_t)yaml_get);
    service_register(plugin, "destroy", (intptr_t)yaml_destroy);
}
