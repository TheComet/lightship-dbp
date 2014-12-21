#include "lightship/api.h"
#include "main_loop/main_loop.h"

EVENT_C(evt_render)

void register_events(struct plugin_t* plugin, struct lightship_api_t* api)
{
    evt_render = api->event_create(plugin, "render");
}

void register_event_listeners(struct plugin_t* plugin, struct lightship_api_t* api)
{
    api->event_register_listener(plugin, "renderer_gl.close_window", main_loop_stop);
}
