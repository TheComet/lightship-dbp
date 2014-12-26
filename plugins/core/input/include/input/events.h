#include "util/event_api.h"

/* -----------------------------------------------------
 * All events this plugin emits
 * ---------------------------------------------------*/
 
EVENT_H(evt_mouse_clicked)

struct plugin_t;

void register_events(struct plugin_t* plugin);
void register_event_listeners(struct plugin_t* plugin);
