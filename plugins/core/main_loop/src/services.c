#include "lightship/api.h"
#include "main_loop/main_loop.h"

void register_services(struct plugin_t* plugin, struct lightship_api_t* api)
{
    api->service_register(plugin, "start", (intptr_t)main_loop_start);
    api->service_register(plugin, "stop", (intptr_t)main_loop_stop);
}
