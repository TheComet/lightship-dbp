#include "framework/events.h"
#include "plugin_renderer_gl/config.h"
#include "plugin_renderer_gl/events.h"
#include "plugin_renderer_gl/window.h"

struct event_t* evt_close_window;
struct event_t* evt_key_press;
struct event_t* evt_key_release;
struct event_t* evt_mouse_move;
struct event_t* evt_mouse_button_press;
struct event_t* evt_mouse_button_release;
struct event_t* evt_mouse_scroll;

void
register_events(struct plugin_t* plugin)
{
    EVENT_CREATE0(plugin, evt_close_window,         PLUGIN_NAME ".close_window");
    EVENT_CREATE1(plugin, evt_key_press,            PLUGIN_NAME ".key_press", uint32_t);
    EVENT_CREATE1(plugin, evt_key_release,          PLUGIN_NAME ".key_release", uint32_t);
    EVENT_CREATE2(plugin, evt_mouse_move,           PLUGIN_NAME ".mouse_move", double, double);
    EVENT_CREATE1(plugin, evt_mouse_button_press,   PLUGIN_NAME ".mouse_button_press", uint32_t);
    EVENT_CREATE1(plugin, evt_mouse_button_release, PLUGIN_NAME ".mouse_button_release", uint32_t);
    EVENT_CREATE2(plugin, evt_mouse_scroll,         PLUGIN_NAME ".mouse_scroll", uint32_t, uint32_t);
}

void
register_event_listeners(struct plugin_t* plugin)
{
    event_register_listener(plugin->game, "render", on_render);
}
