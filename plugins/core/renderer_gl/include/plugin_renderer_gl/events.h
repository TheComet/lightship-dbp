#include "util/pstdint.h"
#include "framework/events.h"

struct plugin_t;

extern struct event_t* evt_close_window;
extern struct event_t* evt_key_press;
extern struct event_t* evt_key_release;
extern struct event_t* evt_mouse_move;
extern struct event_t* evt_mouse_button_press;
extern struct event_t* evt_mouse_button_release;
extern struct event_t* evt_mouse_scroll;

void
register_events(struct plugin_t* plugin);

void
register_event_listeners(struct plugin_t* plugin);
