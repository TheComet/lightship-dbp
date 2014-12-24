#include "util/events.h"
#include "renderer_gl/window.h"

EVENT_C(evt_close_window)

void register_events(struct plugin_t* plugin)
{
    evt_close_window = event_create(plugin, "close_window");
}

void register_event_listeners(struct plugin_t* plugin)
{
    event_register_listener(plugin, "main_loop.render", on_render);
}
