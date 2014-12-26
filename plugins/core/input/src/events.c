#include "util/events.h"
#include "input/events.h"
#include "input/mouse.h"
#include "input/keyboard.h"

/* -----------------------------------------------------
 * All events this plugin emits
 * ---------------------------------------------------*/

EVENT_C(evt_mouse_clicked)
EVENT_H(evt_mouse_press)
EVENT_C(evt_mouse_release)
EVENT_C(evt_mouse_move)
EVENT_C(evt_up)
EVENT_C(evt_down)
EVENT_C(evt_left)
EVENT_C(evt_right)
EVENT_C(evt_action)
EVENT_C(evt_cancel)

void register_events(struct plugin_t* plugin)
{
    evt_mouse_clicked   = event_create(plugin, "mouse_clicked");
    evt_mouse_press     = event_create(plugin, "mouse_press");
    evt_mouse_release   = event_create(plugin, "mouse_release");
    evt_mouse_move      = event_create(plugin, "mouse_move");
    evt_up              = event_create(plugin, "up");
    evt_down            = event_create(plugin, "down");
    evt_left            = event_create(plugin, "left");
    evt_right           = event_create(plugin, "right");
    evt_action          = event_create(plugin, "action");
    evt_cancel          = event_create(plugin, "cancel");
}

void register_event_listeners(struct plugin_t* plugin)
{
    event_register_listener(plugin, "renderer_gl.mouse_button_press", on_mouse_button_press);
    event_register_listener(plugin, "renderer_gl.mouse_button_release", on_mouse_button_release);
    event_register_listener(plugin, "renderer_gl.key_press", on_key_press);
    event_register_listener(plugin, "renderer_gl.key_release", on_key_release);
    
}
