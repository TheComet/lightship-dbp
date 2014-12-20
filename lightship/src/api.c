#include <string.h>
#include "lightship/api.h"
#include "lightship/plugin_manager.h"
#include "lightship/services.h"
#include "lightship/events.h"

struct lightship_api_t g_api;

void api_init(void)
{
    memset(&g_api, 0, sizeof(struct lightship_api_t));
    
    g_api.plugin_load = plugin_load;
    g_api.plugin_unload = plugin_unload;
    g_api.plugin_get_by_name = plugin_get_by_name;
    g_api.service_register = service_register;
    g_api.service_unregister = service_unregister;
    g_api.service_get = service_get;
    g_api.event_register = event_register;
    g_api.event_unregister = event_unregister;
    g_api.event_get = event_get;
}
