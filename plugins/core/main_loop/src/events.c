#include "util/events.h"
#include "main_loop/main_loop.h"

EVENT_C(evt_render)

void register_events(struct plugin_t* plugin)
{
    evt_render = event_create(plugin, "render");
}

void register_event_listeners(struct plugin_t* plugin)
{
    event_register_listener(plugin, "renderer_gl.close_window", main_loop_stop);
}
