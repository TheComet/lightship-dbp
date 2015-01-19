#include "util/event_api.h"

/* -----------------------------------------------------
 * All events this plugin emits
 * ---------------------------------------------------*/
 
EVENT_H(evt_button_clicked)

struct plugin_t;

void
register_events(const struct plugin_t* plugin);

void
register_event_listeners(const struct plugin_t* plugin);
