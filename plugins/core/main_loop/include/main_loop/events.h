#include "util/event_api.h"

struct plugin_t;
struct lightship_api_t;

EVENT(evt_render);

void register_events(struct plugin_t* plugin, struct lightship_api_t* api);
void register_event_listeners(struct plugin_t* plugin, struct lightship_api_t* api);
