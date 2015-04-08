#include "util/pstdint.h"
#include "framework/event_api.h"

struct plugin_t;

/* events this plugin defines */
EVENT_H0(evt_close_window)
EVENT_H1(evt_key_press, uint32_t)
EVENT_H1(evt_key_release, uint32_t)
EVENT_H2(evt_mouse_move, double, double)
EVENT_H1(evt_mouse_button_press, uint32_t)
EVENT_H1(evt_mouse_button_release, uint32_t)
EVENT_H2(evt_mouse_scroll, uint32_t, uint32_t)

void
register_events(struct game_t* game, struct plugin_t* plugin);

void
register_event_listeners(struct game_t* game, struct plugin_t* plugin);
