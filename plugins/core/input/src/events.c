#include "framework/events.h"
#include "framework/plugin.h"
#include "plugin_input/events.h"
#include "plugin_input/mouse.h"
#include "plugin_input/keyboard.h"

/* -----------------------------------------------------
 * All events this plugin emits
 * ---------------------------------------------------*/

EVENT_C3(evt_mouse_clicked, char, double, double)
EVENT_C0(evt_mouse_press)
EVENT_C0(evt_mouse_release)
EVENT_C0(evt_mouse_move)
EVENT_C0(evt_up)
EVENT_C0(evt_down)
EVENT_C0(evt_left)
EVENT_C0(evt_right)
EVENT_C0(evt_action)
EVENT_C0(evt_cancel)

void
register_events(struct plugin_t* plugin)
{
    struct game_t* game = plugin->game;
    evt_mouse_clicked   = event_create(game, plugin, "mouse_clicked");
    evt_mouse_press     = event_create(game, plugin, "mouse_press");
    evt_mouse_release   = event_create(game, plugin, "mouse_release");
    evt_mouse_move      = event_create(game, plugin, "mouse_move");
    evt_up              = event_create(game, plugin, "up");
    evt_down            = event_create(game, plugin, "down");
    evt_left            = event_create(game, plugin, "left");
    evt_right           = event_create(game, plugin, "right");
    evt_action          = event_create(game, plugin, "action");
    evt_cancel          = event_create(game, plugin, "cancel");
}

void
register_event_listeners(struct plugin_t* plugin)
{
    struct game_t* game = plugin->game;
    event_register_listener(game, plugin, "renderer_gl.mouse_move", on_mouse_move);
    event_register_listener(game, plugin, "renderer_gl.mouse_button_press", on_mouse_button_press);
    event_register_listener(game, plugin, "renderer_gl.mouse_button_release", on_mouse_button_release);
    event_register_listener(game, plugin, "renderer_gl.key_press", on_key_press);
    event_register_listener(game, plugin, "renderer_gl.key_release", on_key_release);
    
}
