#include "plugin_manager/event_api.h"

struct plugin_t;

EVENT_H0(evt_render);
EVENT_H0(evt_update);
EVENT_H2(evt_stats, uint32_t, uint32_t);

void
register_events(const struct plugin_t* plugin);

void
register_event_listeners(const struct plugin_t* plugin);
