#include "framework/events.h"

struct plugin_t;

/* -----------------------------------------------------
 * All events this plugin emits
 * ---------------------------------------------------*/

extern struct event_t* evt_mouse_clicked;
extern struct event_t* evt_mouse_press;
extern struct event_t* evt_mouse_release;
extern struct event_t* evt_mouse_move;
extern struct event_t* evt_up;
extern struct event_t* evt_down;
extern struct event_t* evt_left;
extern struct event_t* evt_right;
extern struct event_t* evt_action;
extern struct event_t* evt_cancel;

void
register_events(struct plugin_t* plugin);

void
register_event_listeners(struct plugin_t* plugin);
