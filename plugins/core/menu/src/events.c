#include "plugin_menu/events.h"
#include "plugin_menu/button.h"
#include "plugin_manager/events.h"
#include "plugin_manager/plugin.h"

/* -----------------------------------------------------
 * All events this plugin emits
 * ---------------------------------------------------*/

EVENT_C1(evt_button_clicked, uint32_t);

void
register_events(struct plugin_t* plugin)
{
    struct game_t* game = plugin->game;
    /*evt_button_clicked = event_create(game, plugin, "button_clicked");*/
}

void
register_event_listeners(struct plugin_t* plugin)
{
    struct game_t* game = plugin->game;
    event_register_listener(game, plugin, "input.mouse_clicked", (event_callback_func)on_mouse_clicked);
}
