#include "plugin_manager/events.h"
#include "plugin_renderer_gl/events.h"
#include "plugin_renderer_gl/window.h"

EVENT_C0(evt_close_window)
EVENT_C1(evt_key_press, uint32_t)
EVENT_C1(evt_key_release, uint32_t)
EVENT_C2(evt_mouse_move, double, double)
EVENT_C1(evt_mouse_button_press, uint32_t)
EVENT_C1(evt_mouse_button_release, uint32_t)
EVENT_C2(evt_mouse_scroll, uint32_t, uint32_t)

void
register_events(struct game_t* game, struct plugin_t* plugin)
{
    evt_close_window            = event_create(game, plugin, "close_window");
    evt_key_press               = event_create(game, plugin, "key_press");
    evt_key_release             = event_create(game, plugin, "key_release");
    evt_mouse_move              = event_create(game, plugin, "mouse_move");
    evt_mouse_button_press      = event_create(game, plugin, "mouse_button_press");
    evt_mouse_button_release    = event_create(game, plugin, "mouse_button_release");
    evt_mouse_scroll            = event_create(game, plugin, "mouse_scroll");
}

void
register_event_listeners(struct game_t* game, struct plugin_t* plugin)
{
    event_register_listener(game, plugin, "main_loop.render", on_render);
}
