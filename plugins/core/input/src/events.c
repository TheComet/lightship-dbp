#include "util/events.h"
#include "input/events.h"
#include "input/mouse.h"

/* -----------------------------------------------------
 * All events this plugin emits
 * ---------------------------------------------------*/

EVENT_C(evt_mouse_clicked)

void register_events(struct plugin_t* plugin)
{
    evt_mouse_clicked = event_create(plugin, "mouse_clicked");
}

void register_event_listeners(struct plugin_t* plugin)
{
    event_register_listener(plugin, "renderer_gl.mouse_button_press", on_mouse_button_press);
    event_register_listener(plugin, "renderer_gl.mouse_button_release", on_mouse_button_release);
}
