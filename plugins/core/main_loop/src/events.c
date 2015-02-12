#include "plugin_manager/events.h"
#include "plugin_main_loop/events.h"
#include "plugin_main_loop/main_loop.h"

EVENT_C0(evt_render);
EVENT_C0(evt_update);
EVENT_C2(evt_stats, uint32_t, uint32_t);
EVENT_C0(evt_start);
EVENT_C0(evt_stop);

void register_events(struct game_t* game, struct plugin_t* plugin)
{
    evt_render = event_create(game, plugin, "render");
    evt_update = event_create(game, plugin, "update");
    evt_stats  = event_create(game, plugin, "stats");
    evt_start  = event_create(game, plugin, "start");
    evt_stop   = event_create(game, plugin, "stop");
}

void register_event_listeners(struct game_t* game, struct plugin_t* plugin)
{
    event_register_listener(game, plugin, "renderer_gl.close_window", on_main_loop_stop);
#ifdef _DEBUG
    event_register_listener(game, plugin, "main_loop.stats", on_stats);
#endif
}
