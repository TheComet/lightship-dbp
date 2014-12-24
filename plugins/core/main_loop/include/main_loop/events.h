#include "util/event_api.h"

struct plugin_t;

EVENT_H(evt_render)

void register_events(struct plugin_t* plugin);
void register_event_listeners(struct plugin_t* plugin);
