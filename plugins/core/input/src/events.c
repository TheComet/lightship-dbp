#include "framework/events.h"
#include "framework/plugin.h"
#include "plugin_input/config.h"
#include "plugin_input/events.h"
#include "plugin_input/mouse.h"
#include "plugin_input/keyboard.h"

struct event_t* evt_mouse_clicked;
struct event_t* evt_mouse_press;
struct event_t* evt_mouse_release;
struct event_t* evt_mouse_move;
struct event_t* evt_up;
struct event_t* evt_down;
struct event_t* evt_left;
struct event_t* evt_right;
struct event_t* evt_action;
struct event_t* evt_cancel;

void
register_events(struct plugin_t* plugin)
{
    EVENT_CREATE3(plugin, evt_mouse_clicked, PLUGIN_NAME ".mouse_clicked", char, double, double);
    EVENT_CREATE0(plugin, evt_mouse_press  , PLUGIN_NAME ".mouse_press");
    EVENT_CREATE0(plugin, evt_mouse_release, PLUGIN_NAME ".mouse_release");
    EVENT_CREATE0(plugin, evt_mouse_move   , PLUGIN_NAME ".mouse_move");
    EVENT_CREATE0(plugin, evt_up           , PLUGIN_NAME ".up");
    EVENT_CREATE0(plugin, evt_down         , PLUGIN_NAME ".down");
    EVENT_CREATE0(plugin, evt_left         , PLUGIN_NAME ".left");
    EVENT_CREATE0(plugin, evt_right        , PLUGIN_NAME ".right");
    EVENT_CREATE0(plugin, evt_action       , PLUGIN_NAME ".action");
    EVENT_CREATE0(plugin, evt_cancel       , PLUGIN_NAME ".cancel");
}

void
register_event_listeners(struct plugin_t* plugin)
{
    struct game_t* game = plugin->game;
    event_register_listener(game, "renderer_gl.mouse_move", on_mouse_move);
    event_register_listener(game, "renderer_gl.mouse_button_press", on_mouse_button_press);
    event_register_listener(game, "renderer_gl.mouse_button_release", on_mouse_button_release);
    event_register_listener(game, "renderer_gl.key_press", on_key_press);
    event_register_listener(game, "renderer_gl.key_release", on_key_release);
}
