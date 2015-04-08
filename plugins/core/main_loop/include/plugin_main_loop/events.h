#include "framework/event_api.h"

struct plugin_t;
struct game_t;

EVENT_H_NO_EXTERN0(evt_render)
EVENT_H_NO_EXTERN0(evt_update)
EVENT_H_NO_EXTERN2(evt_stats, uint32_t, uint32_t)
EVENT_H_NO_EXTERN0(evt_start)
EVENT_H_NO_EXTERN0(evt_stop)

void
register_events(struct plugin_t* plugin);

void
register_event_listeners(struct plugin_t* plugin);
