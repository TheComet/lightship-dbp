#include "framework/events.h"
#include "util/pstdint.h"

/* -----------------------------------------------------
 * All events this plugin fires
 * ---------------------------------------------------*/

struct plugin_t;

void
register_events(struct plugin_t* plugin);

void
register_event_listeners(struct plugin_t* plugin);
