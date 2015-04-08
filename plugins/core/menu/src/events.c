#include "plugin_menu/events.h"
#include "plugin_menu/button.h"
#include "plugin_menu/glob.h"
#include "framework/events.h"
#include "framework/plugin.h"
#include <string.h>

/* -----------------------------------------------------
 * All events this plugin emits
 * ---------------------------------------------------*/

EVENT_C1(evt_button_clicked, uint32_t)

void
register_events(struct plugin_t* plugin)
{
    struct game_t* game = plugin->game;
    
    /* get events struct and initialise all event pointers to NULL */
    struct glob_events_t* g = &get_global(game)->events;
    memset(g, 0, sizeof(struct glob_events_t));

    g->button_clicked = event_create(game, plugin, "button_clicked");
}

void
register_event_listeners(struct plugin_t* plugin)
{
    struct game_t* game = plugin->game;
    event_register_listener(game, plugin, "input.mouse_clicked", (event_callback_func)on_mouse_clicked);
}
