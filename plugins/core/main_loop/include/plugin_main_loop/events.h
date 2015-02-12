#include "plugin_manager/event_api.h"

struct plugin_t;
struct game_t;

EVENT_H0(evt_render)
EVENT_H0(evt_update)
EVENT_H2(evt_stats, uint32_t, uint32_t)
EVENT_H0(evt_start)
EVENT_H0(evt_stop)

void
register_events(struct game_t* game, struct plugin_t* plugin);

void
register_event_listeners(struct game_t* game, struct plugin_t* plugin);
