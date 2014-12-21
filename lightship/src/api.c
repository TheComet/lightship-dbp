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
    g_api.event_create = event_create;
    g_api.event_destroy = event_destroy;
    g_api.event_destroy_plugin_event = event_destroy_plugin_event;
    g_api.event_destroy_all_plugin_events = event_destroy_all_plugin_events;
    g_api.event_get = event_get;
    g_api.event_register_listener = event_register_listener;
    g_api.event_unregister_listener = event_unregister_listener;
    g_api.event_unregister_all_listeners = event_unregister_all_listeners;
}
