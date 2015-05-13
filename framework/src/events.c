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

/* ----------------------------------------------------------------------------
 * Exported functions
 * ------------------------------------------------------------------------- */
char
events_register_core_events(struct game_t* game)
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
         * Define a macro, so I don't have to type that much.
         * This will register an event to the "game" object with the specified
         * name, and save the returned event object into game->event.(name). If
         * anything fails, break is called.
         */
#define STR_(x) #x
#define STR(x) STR_(x)
#define REGISTER_BUILT_IN_EVENT(name) {                                     \
            if(!(game->event.name = event_create(game->core, STR(name))))   \
                break; }

        /* game core commands */
        REGISTER_BUILT_IN_EVENT(start)
        REGISTER_BUILT_IN_EVENT(pause)
        REGISTER_BUILT_IN_EVENT(exit)

        /* main loop events (game update and render updates) */
        REGISTER_BUILT_IN_EVENT(tick)
        REGISTER_BUILT_IN_EVENT(render)
        REGISTER_BUILT_IN_EVENT(stats)

        /* The log will fire these events appropriately whenever something is logged */
        REGISTER_BUILT_IN_EVENT(log)
        REGISTER_BUILT_IN_EVENT(log_indent)
        REGISTER_BUILT_IN_EVENT(log_unindent)

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
    /* TODO free all events */
}

/* ------------------------------------------------------------------------- */
struct event_t*
event_create(struct plugin_t* plugin, const char* directory)
{
    struct ptree_t* node;
    struct event_t* event;

    assert(plugin);
    assert(plugin->game);
    assert(directory);

    /* allocate and initialise event object */
    if(!(event = (struct event_t*)MALLOC(sizeof(struct event_t))))
        OUT_OF_MEMORY("event_create()", NULL);
    memset(event, 0, sizeof(struct event_t));

    for(;;)
    {
        event->game = plugin->game;
        unordered_vector_init_vector(&event->listeners, sizeof(struct event_listener_t));

        /* copy directory */
        if(!(event->directory = malloc_string(directory)))
            break;

        /* plugin object keeps track of all created events */
        if(!unordered_vector_push(&plugin->events, &event))
            break;

        /* create node in game's event directory and add event - do this last
         * because ptree_remove_node() uses malloc() */
        if(!(node = ptree_add_node(&plugin->game->events, directory, event)))
            break;

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
    assert(event->game);
    assert(event->directory);

    if(!(node = ptree_get_node(&event->game->events, event->directory)))
    {
        llog(LOG_ERROR, event->game, NULL, 5, "Attempted to destroy the event"
            " \"", event->directory, "\", but the associated game object with"
            " name \"", event->game->name, "\" doesn't own it! The event will"
            " not be destroyed.");
        return;
    }

    /* unlink value and destroy node */
    node->value = NULL;
    ptree_destroy(node);

    event_free(event);
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
        llog(LOG_WARNING, game, NULL, 3, "Tried to register as a listener to "
            "event \"", event_directory, "\", but the event does not exist.");
        return 0;
    }

    /* make sure listener hasn't already registered to this event */
    { UNORDERED_VECTOR_FOR_EACH(&event->listeners, struct event_listener_t, listener)
    {
        if(listener->exec == callback)
        {
            llog(LOG_WARNING, game, NULL, 3, "Already registered as a listener"
                " to event \"", event->directory, "\"");
            return 0;
        }
    }}

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
        llog(LOG_WARNING, game, NULL, 3, "Tried to unregister from event \"",
            event_directory, "\", but the event does not exist.");
        return 0;
    }

    { UNORDERED_VECTOR_FOR_EACH(&event->listeners, struct event_listener_t, listener)
    {
        if(listener->exec == callback)
        {
            unordered_vector_erase_element(&event->listeners, listener);
            return 1;
        }
    }}

    llog(LOG_WARNING, game, NULL, 3, "Tried to unregister from event \"",
        event_directory, "\", but the listener was not found.");

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
    FREE(event);
}
