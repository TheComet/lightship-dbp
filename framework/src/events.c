#include "framework/config.h"
#include "framework/events.h"
#include "framework/game.h"
#include "framework/plugin.h"
#include "framework/log.h"
#include "util/hash.h"
#include "util/memory.h"
#include "util/string.h"
#include "util/ptree.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* ----------------------------------------------------------------------------
 * Static functions
 * ------------------------------------------------------------------------- */

/*!
 * the specified event.
 * @param event The event to unregister the listeners from.
 * @param name_space The name_space to search for.
 */
static void
event_unregister_all_listeners_of_name_space(struct event_t* event,
                                            const char* name_space);

/*!
 * @brief Returns the full name of the event using a plugin object and event name.
 * @note The returned string must be freed manually.
 */
static char*
event_get_full_name(const struct plugin_t* plugin, const char* name);

/*!
 * @brief Returns the name space name of the event using a plugin object.
 * @note The returned string must be freed manually.
 */
static char*
event_get_name_space_name(const struct plugin_t* plugin);

/*!
 * @brief Frees an event object.
 * @note This does not remove it from the list.
 */
static void
event_free(struct event_t* event);

/*!
 * @brief Frees an event listener object.
 * @note This does not remove it from the list.
 */
static void
event_listener_free_contents(struct event_listener_t* listener);

/*!
 * @brief Allocates and registers an event globally with the specified name.
 * @note No checks for duplicates are performed. This is an internal function.
 * @param[in] full_name The full name of the event.
 * @note The event object owns **full_name** after this call and will free it
 * when the event is destroyed.
 * @return The new event object.
 */
static struct event_t*
event_malloc_and_register(struct game_t* game, char* full_name);

/* ----------------------------------------------------------------------------
 * Exported functions
 * ------------------------------------------------------------------------- */
char
events_init(struct game_t* game)
{
    assert(game);
    
    /* this holds all of the game's events */
    ptree_init_ptree(&game->events);
    
    /* ----------------------------
     * Register built-in events 
     * --------------------------*/
    
    for(;;)
    {
        /*
         * Define a macro, so I don't have to type that much.
         * This will register an event to the "game" object with the specified
         * name, and save the returned event object into game->event.(name).
         */
#define STR_(x) #x
#define STR(x) STR_(x)
#define REGISTER_BUILT_IN_EVENT(name) {                             \
            char* name_str;                                         \
            if(!(name_str = malloc_string(STR(name)))) break;       \
            if(!(game->event.name =                                 \
                event_malloc_and_register(game, name_str))) break; }

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
    
    events_deinit(game);
    
    return 0;
}

/* ------------------------------------------------------------------------- */
void
events_deinit(struct game_t* game)
{
    MAP_FOR_EACH(&game->events, struct event_t, key, event)
    {
        event_free(event);
    }
    map_clear_free(&game->events);
}

/* ------------------------------------------------------------------------- */
struct event_t*
event_create(struct game_t* game, const char* name)
{

    /* check for duplicate event names */
    char* full_name = event_get_full_name(plugin, name);
    if(event_get(game, full_name))
    {
        free_string(full_name);
        return NULL;
    }
    
    return event_malloc_and_register(game, full_name);
}

/* ------------------------------------------------------------------------- */
void
event_destroy(struct event_t* event)
{
    assert(event);
    
    if(!map_erase_element(&event->game->events, event))
        llog(LOG_WARNING, event->game, NULL, 1, "Destroying an event that"
            "could not be found in the associated game object");

    event_free(event);
}

/* ------------------------------------------------------------------------- */
void
event_destroy_plugin_event(struct game_t* game, const struct plugin_t* plugin, const char* name)
{
    char* full_name;
    uint32_t hash;
    struct event_t* event;
    
    assert(game);
    assert(plugin);
    assert(name);
    
    full_name = event_get_full_name(plugin, name);
    hash = hash_jenkins_oaat(full_name, strlen(full_name));
    free_string(full_name);
    if(!(event = map_erase(&game->events, hash)))
        return;

    event_free(event);
}

/* ------------------------------------------------------------------------- */
void
event_destroy_all_plugin_events(const struct plugin_t* plugin)
{
    char* name_space;
    int len;

    assert(plugin);
    assert(plugin->game);

    name_space = event_get_name_space_name(plugin);
    len = strlen(name_space);
    { MAP_FOR_EACH(&plugin->game->events, struct event_t, key, event)
    {
        if(strncmp(event->name, name_space, len) == 0)
        {
            event_free(event);
            MAP_ERASE_CURRENT_ITEM_IN_FOR_LOOP(&plugin->game->events);
        }
    }}
    free_string(name_space);
}

/* ------------------------------------------------------------------------- */
struct event_t*
event_get(const struct game_t* game, const char* full_name)
{
    uint32_t hash;
    
    assert(game);
    assert(full_name);
    
    hash = hash_jenkins_oaat(full_name, strlen(full_name));
    return map_find(&game->events, hash);
}

/* ------------------------------------------------------------------------- */
char
event_register_listener(const struct game_t* game,
                        const char* event_full_name,
                        event_callback_func callback)
{
    struct event_t* event;
    struct event_listener_t* new_listener;
    char* registering_name_space;
    
    assert(game);
    assert(event_full_name);
    assert(callback);
    
    /* get name space name - if NULL was specified as a plugin, make it builtin */
    if(plugin)
        registering_name_space = plugin->info.name;
    else
        registering_name_space = BUILTIN_NAMESPACE_NAME;
    
    /* make sure event exists */
    if(!(event = event_get(game, event_full_name)))
    {
        llog(LOG_WARNING, game, plugin->info.name, 3, "Tried to register as a listener to event \"",
             event_full_name, "\", but the event does not exist.");
        return 0;
    }
    
    /* make sure plugin hasn't already registered to this event */
    if(plugin)
    {
        UNORDERED_VECTOR_FOR_EACH(&event->listeners, struct event_listener_t, listener)
        {
            if(strcmp(listener->name_space, registering_name_space) == 0)
            {
                llog(LOG_WARNING, game, plugin->info.name, 3, "Already registered as a listener to event \"",
                     event_full_name, "\"");
                return 0;
            }
        }
    }
    
    /* create event listener object */
    new_listener = (struct event_listener_t*)unordered_vector_push_emplace(&event->listeners);
    new_listener->exec = callback;
    /* create and copy string from plugin name */
    new_listener->name_space = cat_strings(2, registering_name_space, ".");
    
    return 1;
}

/* ------------------------------------------------------------------------- */
char
event_unregister_listener(const struct game_t* game,
                          const char* event_name,
                          event_callback_func callback)
{
    struct event_t* event;
    
    if(!(event = event_get(game, event_name)))
        return 0;
    
    {
        UNORDERED_VECTOR_FOR_EACH(&event->listeners, struct event_listener_t, listener)
        {
            if(strcmp(listener->name_space, plugin_name) == 0)
            {
                event_listener_free_contents(listener);
                unordered_vector_erase_element(&event->listeners, listener);
                return 1;
            }
        }
    }
    
    return 0;
}

/* ------------------------------------------------------------------------- */
void
event_unregister_all_listeners(struct event_t* event)
{
    UNORDERED_VECTOR_FOR_EACH(&event->listeners, struct event_listener_t, listener)
    {
        event_listener_free_contents(listener);
    }
    unordered_vector_clear_free(&event->listeners);
}

/* ----------------------------------------------------------------------------
 * Static functions
 * ------------------------------------------------------------------------- */
static struct event_t*
event_malloc_and_register(struct game_t* game, char* full_name)
{
    struct event_t* event;
    uint32_t hash;
    
    /* create new event and register to global list of events */
    event = (struct event_t*)MALLOC(sizeof(struct event_t));
    if(!event)
        OUT_OF_MEMORY("event_malloc_and_register()", NULL);
    event->name = full_name;
    event->game = game;
    unordered_vector_init_vector(&event->listeners, sizeof(struct event_listener_t));
    
    /* add event to game object */
    hash = hash_jenkins_oaat(full_name, strlen(full_name));
    map_insert(&game->events, hash, event);
    return event;
}

/* ------------------------------------------------------------------------- */
static void
event_unregister_all_listeners_of_name_space(struct event_t* event,
                                             const char* name_space)
{
    int len = strlen(name_space);
    {
        UNORDERED_VECTOR_FOR_EACH(&event->listeners, struct event_listener_t, listener)
        {
            if(strncmp(listener->name_space, name_space, len) == 0)
            {
                event_listener_free_contents(listener);
                UNORDERED_VECTOR_ERASE_IN_FOR_LOOP(&event->listeners, struct event_listener_t, listener);
            }
        }
    }
}

/* ------------------------------------------------------------------------- */
static char*
event_get_full_name(const struct plugin_t* plugin, const char* name)
{
    return cat_strings(3, plugin->info.name, ".", name);
}

/* ------------------------------------------------------------------------- */
static char*
event_get_name_space_name(const struct plugin_t* plugin)
{
    return cat_strings(2, plugin->info.name, ".");
}

/* ------------------------------------------------------------------------- */
static void
event_free(struct event_t* event)
{
    event_unregister_all_listeners(event);
    free_string(event->name);
    unordered_vector_clear_free(&event->listeners);
    FREE(event);
}

/* ------------------------------------------------------------------------- */
static void
event_listener_free_contents(struct event_listener_t* listener)
{
    free_string(listener->name_space);
}
