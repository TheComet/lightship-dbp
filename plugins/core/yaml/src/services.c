#include "util/services.h"
#include "plugin_yaml/services.h"
#include "plugin_yaml/parser.h"

void register_services(struct plugin_t* plugin)
{
    /* -----------------------------------------------------
    * All services this plugin supports
    * ---------------------------------------------------*/

    service_register(plugin, "open", (intptr_t)yaml_open);
    service_register(plugin, "close", (intptr_t)yaml_close);
}
