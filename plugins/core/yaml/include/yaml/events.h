#include "util/event_api.h"

/* -----------------------------------------------------
 * All events this plugin emits
 * ---------------------------------------------------*/
 
 /* 
  * EVENT_H(evt_name_1)
  * EVENT_H(evt_name_2)
  * etc...
  */

struct plugin_t;

void register_events(struct plugin_t* plugin);
void register_event_listeners(struct plugin_t* plugin);
