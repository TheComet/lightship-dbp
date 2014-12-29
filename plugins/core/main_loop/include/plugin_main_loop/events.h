#include "util/event_api.h"

struct plugin_t;

EVENT_H(evt_render)
EVENT_H(evt_update)
EVENT_H(evt_stats)

void
register_events(const struct plugin_t* plugin);

void
register_event_listeners(const struct plugin_t* plugin);
