#include "util/event_api.h"

struct plugin_t;

/* events this plugin defines */
EVENT_H(evt_close_window)

void register_events(struct plugin_t* plugin);
void register_event_listeners(struct plugin_t* plugin);
