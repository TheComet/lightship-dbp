#include "framework/config.h"
#include "framework/events.h"
#include "framework/game.h"
#include "framework/plugin.h"
#include "framework/log.h"
#include "util/hash.h"
#include "util/memory.h"
#include "util/string.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* ----------------------------------------------------------------------------
 * Static functions
 * ------------------------------------------------------------------------- */

/*!
 * @brief Frees an event object.
 * @note This does not remove it from the list.
 */
static void
event_free(struct event_t* event);

/*!
 * @brief Same as event_create, but it doesn't fire the "event.created" event.
 */
static struct event_t*
event_create_no_fire_notification(struct plugin_t* plugin,
								  const char* directory,
								  struct type_info_t* type_info);

/* ----------------------------------------------------------------------------
 * Exported functions
 * ------------------------------------------------------------------------- */
char
events_init(struct game_t* game)
{
	assert(game);
	assert(game->core);

	/* this holds all of the game's events */
	ptree_init_ptree(&game->events, NULL);

	/* ----------------------------
	 * Register built-in events
	 * --------------------------*/

	for(;;)
	{
		/*
		 * Define a macro to make typing easier. Checks if the specified member
		 * of the game object is not null. If it is null, break is called.
		 */
#define CHECK(name) \
		if(!(game->event.name)) break;

		/*
		 * Re-define EVENT_CREATE so the following events do not fire.
		 */
#pragma push_macro("EVENT_CREATE")
#undef EVENT_CREATE
#define EVENT_CREATE event_create_no_fire_notification

		/* game core commands */
		EVENT_CREATE0(game->core, game->event.start, "start"); CHECK(start)
		EVENT_CREATE0(game->core, game->event.pause, "pause"); CHECK(pause)
		EVENT_CREATE0(game->core, game->event.exit,  "exit");  CHECK(exit)

		/* main loop events (game update and render updates) */
		EVENT_CREATE0(game->core, game->event.tick,   "tick");                      CHECK(tick)
		EVENT_CREATE0(game->core, game->event.render, "render");                    CHECK(render)
		EVENT_CREATE2(game->core, game->event.stats,  "stats", uint32_t, uint32_t); CHECK(stats)

		/* The log will fire these events appropriately whenever something is logged */
		EVENT_CREATE2(game->core, game->event.log,          "log", uint32_t, const char*); CHECK(log)
		EVENT_CREATE1(game->core, game->event.log_indent,   "log.indent", const char*);    CHECK(log_indent)
		EVENT_CREATE0(game->core, game->event.log_unindent, "log.unindent");               CHECK(log_unindent)

		/* fired when events or services are created/destroyed */
		EVENT_CREATE1(game->core, game->event.event_created,     "event.created",     const char*); CHECK(event_created);
		EVENT_CREATE1(game->core, game->event.event_destroyed,   "event.destroyed",   const char*); CHECK(event_destroyed);
		EVENT_CREATE1(game->core, game->event.service_created,   "service.created",   const char*); CHECK(service_created);
		EVENT_CREATE1(game->core, game->event.service_destroyed, "service.destroyed", const char*); CHECK(service_destroyed);

#undef EVENT_CREATE
#pragma pop_macro("EVENT_CREATE")
#undef CHECK

		return 1;
	}

	/* ------------------------------------------------------------
	 * Reaching this point means something went wrong - Clean up
	 * --------------------------------------------------------- */

	return 0;
}

/* ------------------------------------------------------------------------- */
void
events_deinit(struct game_t* game)
{
	ptree_destroy_keep_root(&game->events);
}

/* ------------------------------------------------------------------------- */
struct event_t*
event_create(struct plugin_t* plugin,
			 const char* directory,
			 struct type_info_t* type_info)
{
	struct event_t* event;
	event = event_create_no_fire_notification(plugin, directory, type_info);
	if(!event)
		return NULL;

	EVENT_FIRE1(plugin->game->event.event_created, PTR(directory));
	return event;
}

static struct event_t*
event_create_no_fire_notification(struct plugin_t* plugin,
								  const char* directory,
								  struct type_info_t* type_info)
{
	struct ptree_t* node;
	struct event_t* event;

	assert(plugin);
	assert(plugin->game);
	assert(directory);

	/* make sure directory contains valid characters only */
	if(!directory_name_is_valid(directory))
		return NULL;

	/* allocate and initialise event object */
	if(!(event = (struct event_t*)MALLOC(sizeof(struct event_t))))
		OUT_OF_MEMORY("event_create()", NULL);
	memset(event, 0, sizeof(struct event_t));

	for(;;)
	{
		event->plugin = plugin;
		event->type_info = type_info;
		unordered_vector_init_vector(&event->listeners, sizeof(struct event_listener_t));

		/* copy directory */
		if(!(event->directory = malloc_string(directory)))
			break;

		/* plugin object keeps track of all created events */
		if(!unordered_vector_push(&plugin->events, &event))
			break;

		/* create node in game's event directory and add event - do this last
		 * because ptree_remove_node() uses malloc() */
		if(!(node = ptree_set(&plugin->game->events, directory, event)))
			break;

		/* NOTE: don't MALLOC() past this point ----------------------- */

		/* set the node's free function to event_free() to make deleting
		 * nodes easier */
		ptree_set_free_func(node, (ptree_free_func)event_free);

		/* success! */
		return event;
	}

	/* something went wrong, clean up */
	if(event->directory)
		free_string(event->directory);

	return NULL;
}

/* ------------------------------------------------------------------------- */
void
event_destroy(struct event_t* event)
{
	struct ptree_t* node;

	assert(event);
	assert(event->plugin);
	assert(event->plugin->game);
	assert(event->directory);

	if(!(node = ptree_get_node(&event->plugin->game->events, event->directory)))
	{
		llog(LOG_ERROR, event->plugin->game, NULL, "Attempted to destroy the event"
			" \"%s\", but the associated game object with name \"%s\" doesn't "
			"own it! The event will not be destroyed.",
			 event->directory, event->plugin->game->name);
		return;
	}

	EVENT_FIRE1(event->plugin->game->event.event_destroyed,
				PTR(event->directory));

	/* destroying the node will call event_free() automatically */
	ptree_destroy(node);
}

/* ------------------------------------------------------------------------- */
struct event_t*
event_get(const struct game_t* game, const char* directory)
{
	struct ptree_t* node;

	assert(game);
	assert(directory);

	if(!(node = ptree_get_node(&game->events, directory)))
		return NULL;
	/* The node can be NULL if the node is a "middle node". This doesn't
	 * concern us, though, because we would be returning NULL anyway */
	return (struct event_t*)node->value;
}

/* ------------------------------------------------------------------------- */
char
event_register_listener(const struct game_t* game,
						const char* event_directory,
						event_callback_func callback)
{
	struct event_t* event;
	struct event_listener_t* new_listener;

	assert(game);
	assert(event_directory);
	assert(callback);

	/* make sure event exists */
	if(!(event = event_get(game, event_directory)))
	{
		llog(LOG_WARNING, game, NULL, "Tried to register as a listener to "
			"event \"%s\", but the event does not exist.", event_directory);
		return 0;
	}

	/* make sure listener hasn't already registered to this event */
	UNORDERED_VECTOR_FOR_EACH(&event->listeners, struct event_listener_t, listener)
		if(listener->exec == callback)
		{
			llog(LOG_WARNING, game, NULL, "Already registered as a listener"
				" to event \"%s\"", event->directory);
			return 0;
		}
	UNORDERED_VECTOR_END_EACH

	/* create event listener object */
	new_listener = (struct event_listener_t*) unordered_vector_push_emplace(&event->listeners);
	new_listener->exec = callback;

	return 1;
}

/* ------------------------------------------------------------------------- */
char
event_unregister_listener(const struct game_t* game,
						  const char* event_directory,
						  event_callback_func callback)
{
	struct event_t* event;

	if(!(event = event_get(game, event_directory)))
	{
		llog(LOG_WARNING, game, NULL, "Tried to unregister from event \"%s\", "
			"but the event does not exist.", event_directory);
		return 0;
	}

	UNORDERED_VECTOR_FOR_EACH(&event->listeners, struct event_listener_t, listener)
		if(listener->exec == callback)
		{
			unordered_vector_erase_element(&event->listeners, listener);
			return 1;
		}
	UNORDERED_VECTOR_END_EACH

	llog(LOG_WARNING, game, NULL, "Tried to unregister from event \"%s\", but "
		"the listener was not found.", event_directory);

	return 0;
}

/* ------------------------------------------------------------------------- */
void
event_unregister_all_listeners(struct event_t* event)
{
	unordered_vector_clear_free(&event->listeners);
}

/* ----------------------------------------------------------------------------
 * Static functions
 * ------------------------------------------------------------------------- */
static void
event_free(struct event_t* event)
{
	event_unregister_all_listeners(event);
	free_string(event->directory);
	unordered_vector_clear_free(&event->listeners);
	dynamic_call_destroy_type_info(event->type_info);
	FREE(event);
}
