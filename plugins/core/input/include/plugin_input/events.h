#include "util/event_api.h"

/* -----------------------------------------------------
 * All events this plugin emits
 * ---------------------------------------------------*/

EVENT_H(evt_mouse_press)
EVENT_H(evt_mouse_release)
EVENT_H(evt_mouse_move)
EVENT_H(evt_mouse_clicked)
EVENT_H(evt_up)
EVENT_H(evt_down)
EVENT_H(evt_left)
EVENT_H(evt_right)
EVENT_H(evt_action)
EVENT_H(evt_cancel)

struct plugin_t;

void
register_events(const struct plugin_t* plugin);

void
register_event_listeners(const struct plugin_t* plugin);
