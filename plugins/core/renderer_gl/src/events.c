#include "lightship/api.h"
#include "util/event_api.h"
#include "renderer_gl/window.h"

EVENT_C(evt_close_window)

void register_events(struct plugin_t* plugin, struct lightship_api_t* api)
{
    evt_close_window = api->event_create(plugin, "close_window");
}

void register_event_listeners(struct plugin_t* plugin, struct lightship_api_t* api)
{
    api->event_register_listener(plugin, "main_loop.render", on_render);
}
