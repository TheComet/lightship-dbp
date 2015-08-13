#include "framework/events.h"
#include "framework/plugin.h"
#include "plugin_python/events.h"
#include "plugin_python/context.h"

/* ----------------------------------------------------------------------------
 * All events this plugin fires
 * ------------------------------------------------------------------------- */

/*
 * EVENT_C0(evt_name_1)
 * EVENT_C0(evt_name_2)
 * etc...
 */

/* ------------------------------------------------------------------------- */
void
register_events(struct plugin_t* plugin)
{
	/* get event context and set every service pointer to NULL */
	/*
	 * See context.h. All event objects cannot be stored statically. Instead, they
	 * are inserted into a context struct, which allows each plugin to be loaded
	 * multiple times without static data ever conflicting. Every plugin owns
	 * one context struct which can be retrieved from the game object.
	 *
	 * The event objects you want to register can be added to the context struct
	 * in context.h.
	 *
	struct context_events_t* g_events = &get_context(game)->events;
	memset(g_events, 0, sizeof *g_events);*/

	/*
	 * g->name_1 = event_create(game, plugin, "name_1");
	 * g->name_2 = event_create(game, plugin, "name_2");
	 * etc...
	 */
}

/* ------------------------------------------------------------------------- */
void
register_event_listeners(struct plugin_t* plugin)
{
	/*
	 * event_register_listener(game, plugin, "something.event", on_callback_1);
	 * event_register_listener(game, plugin, "something_else.event_2", on_callback_2);
	 * etc...
	 */
}
