#include "plugin_menu/events.h"
#include "plugin_menu/button.h"
#include "util/events.h"

/* -----------------------------------------------------
 * All events this plugin emits
 * ---------------------------------------------------*/

EVENT_C1(evt_button_clicked, wchar_t*);

void
register_events(const struct plugin_t* plugin)
{
    evt_button_clicked = event_create(plugin, "button_clicked");
}

void
register_event_listeners(const struct plugin_t* plugin)
{
    event_register_listener(plugin, "input.mouse_clicked", (event_callback_func)on_mouse_clicked);
}
