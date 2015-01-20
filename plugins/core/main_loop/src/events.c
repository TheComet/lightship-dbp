#include "util/events.h"
#include "plugin_main_loop/main_loop.h"

EVENT_C0(evt_render);
EVENT_C0(evt_update);
EVENT_C2(evt_stats, uint32_t, uint32_t);

void register_events(struct plugin_t* plugin)
{
    evt_render = event_create(plugin, "render");
    evt_update = event_create(plugin, "update");
    evt_stats  = event_create(plugin, "stats");
}

void register_event_listeners(struct plugin_t* plugin)
{
    event_register_listener(plugin, "renderer_gl.close_window", on_main_loop_stop);
#ifdef _DEBUG
    event_register_listener(plugin, "main_loop.stats", on_stats);
#endif
}
