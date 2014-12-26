#include "util/event_api.h"

struct plugin_t;

/* events this plugin defines */
EVENT_H(evt_close_window)
EVENT_H(evt_key_press)
EVENT_H(evt_key_release)
EVENT_H(evt_mouse_move)
EVENT_H(evt_mouse_button_press)
EVENT_H(evt_mouse_button_release)
EVENT_H(evt_mouse_scroll)

void register_events(struct plugin_t* plugin);
void register_event_listeners(struct plugin_t* plugin);
