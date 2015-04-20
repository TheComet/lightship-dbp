#include "framework/events.h"
#include "framework/plugin_api.h"
#include "plugin_main_loop/events.h"
#include "plugin_main_loop/glob.h"
#include <string.h>

EVENT_C0(evt_render)
EVENT_C0(evt_update)
EVENT_C2(evt_stats, uint32_t, uint32_t)
EVENT_C0(evt_start)
EVENT_C0(evt_stop)

void register_events(struct plugin_t* plugin)
{
    struct game_t* game = plugin->game;

    /* get events glob and initialise all event pointers to NULL */
    struct glob_events_t* g = &get_global(game)->events;
    memset(g, 0, sizeof(struct glob_events_t));

    g->render = event_create(game, "render");
    g->update = event_create(game, "update");
    g->stats  = event_create(game, "stats");
    g->start  = event_create(game, "start");
    g->stop   = event_create(game, "stop");
}

void register_event_listeners(struct plugin_t* plugin)
{
    struct game_t* game = plugin->game;
    event_register_listener(game, "main_loop.stop", on_main_loop_stop);
    event_register_listener(game, "renderer_gl.close_window", on_main_loop_stop);
#ifdef _DEBUG
    event_register_listener(game, "main_loop.stats", on_stats);
#endif
}
