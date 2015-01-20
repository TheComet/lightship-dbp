#include "util/event_api.h"

/* -----------------------------------------------------
 * All events this plugin emits
 * ---------------------------------------------------*/

EVENT_H0(evt_mouse_press);
EVENT_H0(evt_mouse_release);
EVENT_H0(evt_mouse_move);
EVENT_H3(evt_mouse_clicked, char, double, double);
EVENT_H0(evt_up);
EVENT_H0(evt_down);
EVENT_H0(evt_left);
EVENT_H0(evt_right);
EVENT_H0(evt_action);
EVENT_H0(evt_cancel);

struct plugin_t;

void
register_events(const struct plugin_t* plugin);

void
register_event_listeners(const struct plugin_t* plugin);
