#include "lightship/events.h"
#include "util/plugin.h"
#include "util/string.h"
#include "util/memory.h"
#include <string.h>
#include <stdlib.h>

/*!
 * @brief Returns the full name of the event using a plugin object and event name.
 * @note The returned string must be FREEd manually.
 */
static char* event_get_full_name(struct plugin_t* plugin, const char* name);

/*!
 * @brief Returns the namespace name of the event using a plugin object.
 * @note The returned string must be FREEd manually.
 */
static char* event_get_namespace_name(struct plugin_t* plugin);

/*!
 * @brief Frees an event object.
 * @note This does not remove it from the list.
 */
static void event_FREE(struct event_t* event);

/*!
 * @brief Frees an event listener object.
 * @note This does not remove it from the list.
 */
static void event_listener_FREE(struct event_listener_t* listener);

struct list_t g_events;

void events_init(void)
{
    list_init_list(&g_events);
}

struct event_t* event_create(struct plugin_t* plugin,
                             const char* name)
{
    char* full_name;
    struct event_t* event;
    
    /* check for duplicate event names */
    full_name = event_get_full_name(plugin, name);
    if(event_get(full_name))
    {
        FREE(full_name);
        return NULL;
    }
    
    /* create new event and register to global list of events */
    event = (struct event_t*)MALLOC(sizeof(struct event_t));
    event->name = full_name; /* full_name must be FREEd */
    event->exec = event_dispatch;
    event->listeners = list_create();
    list_push(&g_events, event);
    
    return event;
}

char event_destroy(struct event_t* event_delete)
{
    LIST_FOR_EACH(&g_events, struct event_t, event)
    {
        if(event == event_delete)
        {
            event_FREE(event);
            list_erase_node(&g_events, node);
            return 1;
        }
    }
    return 0;
}

void event_destroy_plugin_event(struct plugin_t* plugin,
                           const char* name)
{
    char* full_name = event_get_full_name(plugin, name);

    {
        LIST_FOR_EACH(&g_events, struct event_t, event)
        {
            if(strcmp(event->name, full_name) == 0)
            {
                event_FREE(event);
                list_erase_node(&g_events, node);
                break;
            }
        }
    }
    FREE(full_name);
}

void event_destroy_all_plugin_events(struct plugin_t* plugin)
{
    char* namespace = event_get_namespace_name(plugin);
    int len = strlen(namespace) + 1;
    LIST_FOR_EACH_ERASE(&g_events, struct event_t, event)
    {
        if(strncmp(event->name, namespace, len) == 0)
        {
            event_FREE(event);
            list_erase_node(&g_events, node);
        }
    }
}

struct event_t* event_get(const char* full_name)
{
    LIST_FOR_EACH(&g_events, struct event_t, event)
    {
        if(strcmp(event->name, full_name) == 0)
            return event;
    }
    return NULL;
}

char event_register_listener(struct plugin_t* plugin, const char* full_name, event_func callback)
{
    struct event_listener_t* new_listener;
    struct event_t* event = event_get(full_name);
    if(event == NULL)
        return 0;
    
    /* make sure plugin hasn't already registered to this event */
    {
        LIST_FOR_EACH(event->listeners, struct event_listener_t, listener)
        {
            if(strcmp(listener->name, plugin->info.name) == 0)
                return 0;
        }
    }
    
    /* create event listener object */
    new_listener = (struct event_listener_t*)MALLOC(sizeof(struct event_listener_t));
    new_listener->exec = callback;
    /* create and copy string from plugin name */
    new_listener->name = MALLOC_string(plugin->info.name);
    list_push(event->listeners, new_listener);
    
    return 1;
}

char event_unregister_listener(const char* event_name, const char* plugin_name)
{
    struct event_t* event = event_get(event_name);
    if(event == NULL)
        return 0;
    
    {
        LIST_FOR_EACH(event->listeners, struct event_listener_t, listener)
        {
            if(strcmp(listener->name, plugin_name) == 0)
            {
                event_listener_FREE(listener);
                list_erase_node(event->listeners, node);
                return 1;
            }
        }
    }
    
    return 0;
}

void event_unregister_all_listeners(struct event_t* event)
{
    LIST_FOR_EACH_ERASE(event->listeners, struct event_listener_t, listener)
    {
        event_listener_FREE(listener);
        list_erase_node(event->listeners, node);
    }
}

void event_dispatch(struct event_t* event, void* args)
{
    LIST_FOR_EACH(event->listeners, struct event_listener_t, listener)
    {
        listener->exec(event, args);
    }
}

static char* event_get_full_name(struct plugin_t* plugin, const char* name)
{
    return cat_strings(3, plugin->info.name, ".", name);
}

static char* event_get_namespace_name(struct plugin_t* plugin)
{
    return cat_strings(2, plugin->info.name, ".");
}

static void event_FREE(struct event_t* event)
{
    FREE(event->name); /* full_name must be FREEd manually, see event_create() */
    event_unregister_all_listeners(event);
    list_destroy(event->listeners);
    FREE(event);
}

static void event_listener_FREE(struct event_listener_t* listener)
{
    FREE(listener->name);
    FREE(listener);
}
