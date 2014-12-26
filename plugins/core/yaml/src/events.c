#include "yaml/events.h"
#include "util/events.h"

/* -----------------------------------------------------
 * All events this plugin emits
 * ---------------------------------------------------*/

/*
 * EVENT_C(evt_name_1)
 * EVENT_C(evt_name_2)
 * etc...
 */

void register_events(struct plugin_t* plugin)
{
    /* 
     * evt_name_1 = event_create(plugin, "name_1");
     * evt_name_2 = event_create(plugin, "name_2");
     * etc...
     */
}

void register_event_listeners(struct plugin_t* plugin)
{
    /*
     * event_register_listener(plugin, "something.event", on_callback_1);
     * event_register_listener(plugin, "something_else.event_2", on_callback_2);
     * etc...
     */
}
