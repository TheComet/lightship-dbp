#include "util/events.h"
#include "plugin_renderer_gl/window.h"

EVENT_C(evt_close_window)
EVENT_C(evt_key_press)
EVENT_C(evt_key_release)
EVENT_C(evt_mouse_move)
EVENT_C(evt_mouse_button_press)
EVENT_C(evt_mouse_button_release)
EVENT_C(evt_mouse_scroll)

void register_events(struct plugin_t* plugin)
{
    evt_close_window = event_create(plugin, "close_window");
    evt_key_press = event_create(plugin, "key_press");
    evt_key_release = event_create(plugin, "key_release");
    evt_mouse_move = event_create(plugin, "mouse_move");
    evt_mouse_button_press = event_create(plugin, "mouse_button_press");
    evt_mouse_button_release = event_create(plugin, "mouse_button_release");
    evt_mouse_scroll = event_create(plugin, "mouse_scroll");
}

void register_event_listeners(struct plugin_t* plugin)
{
    event_register_listener(plugin, "main_loop.render", on_render);
}
