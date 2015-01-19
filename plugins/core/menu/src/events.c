#include "plugin_menu/events.h"
#include "plugin_menu/button.h"
#include "util/events.h"

/* -----------------------------------------------------
 * All events this plugin emits
 * ---------------------------------------------------*/

EVENT_C(evt_button_clicked);

void
register_events(const struct plugin_t* plugin)
{
    evt_button_clicked = event_create(plugin, "button_clicked");
}

void
register_event_listeners(const struct plugin_t* plugin)
{
    event_register_listener(plugin, "input.mouse_clicked", on_mouse_clicked);
}
