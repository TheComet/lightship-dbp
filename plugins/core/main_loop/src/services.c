#include "util/services.h"
#include "plugin_main_loop/main_loop.h"

void
register_services(struct plugin_t* plugin)
{
    service_register(plugin, "start", (intptr_t)main_loop_start);
    service_register(plugin, "stop",  (intptr_t)main_loop_stop);
}
