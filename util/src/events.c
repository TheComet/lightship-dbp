#include "util/config.h"
#include "util/events.h"
#include "util/plugin.h"
#include "util/string.h"
#include "util/memory.h"
#include <string.h>
#include <stdlib.h>

/*!
 * @brief Unregisters any listeners that belong to the specified namespace from
 * the specified event.
 * @param event The event to unregister the listeners from.
 * @param namespace The namespace to search for.
 */
static void event_unregister_all_listeners_of_namespace(struct event_t* event,
                                                        char* namespace);

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
static void event_free(struct event_t* event);

/*!
 * @brief Frees an event listener object.
 * @note This does not remove it from the list.
 */
static void event_listener_free(struct event_listener_t* listener);

/*!
 * @brief Allocates and registers an event globally with the specified name.
 * @note No checks for duplicates are performed. This is an internal function.
 * @param full_name The full name of the event.
 * @note The event object owns **full_name** after this call and will free it
 * when the event is destroyed.
 * @return The new event object.
 */
static struct event_t* event_malloc_and_register(char* full_name);

struct list_t g_events;
EVENT_C(evt_log)
EVENT_C(evt_log_indent)
EVENT_C(evt_log_unindent)

void events_init(void)
{
    char* name;
    
    list_init_list(&g_events);
    
    /* ----------------------------
     * Register built-in events 
     * --------------------------*/
    
    /* All logging events should be done through this event. */
    name = malloc_string(BUILTIN_NAMESPACE_NAME ".log");
    evt_log = event_malloc_and_register(name);
    name = malloc_string(BUILTIN_NAMESPACE_NAME ".log_indent");
    evt_log_indent = event_malloc_and_register(name);
    name = malloc_string(BUILTIN_NAMESPACE_NAME ".log_unindent");
    evt_log_unindent = event_malloc_and_register(name);
}

void events_deinit(void)
{
    LIST_FOR_EACH_ERASE(&g_events, struct event_t, event)
    {
        event_free(event);
        list_erase_node(&g_events, node);
    }
}

struct event_t* event_create(struct plugin_t* plugin,
                             const char* name)
{

    /* check for duplicate event names */
    char* full_name = event_get_full_name(plugin, name);
    if(event_get(full_name))
    {
        FREE(full_name);
        return NULL;
    }
    
    return event_malloc_and_register(full_name);
}

static struct event_t* event_malloc_and_register(char* full_name)
{
    /* create new event and register to global list of events */
    struct event_t* event = (struct event_t*)MALLOC(sizeof(struct event_t));
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
            event_free(event);
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
                event_free(event);
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
    int len = strlen(namespace);
    LIST_FOR_EACH_ERASE(&g_events, struct event_t, event)
    {
        if(strncmp(event->name, namespace, len) == 0)
        {
            event_free(event);
            list_erase_node(&g_events, node);
        }
    }
    FREE(namespace);
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
    char* register_name;
    
    /* get registration name - if NULL was specified as a plugin, make it builtin */
    if(plugin)
        register_name = plugin->info.name;
    else
        register_name = BUILTIN_NAMESPACE_NAME;
    
    /* make sure event exists */
    struct event_t* event = event_get(full_name);
    if(event == NULL)
        return 0;
    
    /* make sure plugin hasn't already registered to this event */
    if(plugin)
    {
        LIST_FOR_EACH(event->listeners, struct event_listener_t, listener)
        {
            if(strcmp(listener->namespace, register_name) == 0)
                return 0;
        }
    }
    
    /* create event listener object */
    new_listener = (struct event_listener_t*)MALLOC(sizeof(struct event_listener_t));
    new_listener->exec = callback;
    /* create and copy string from plugin name */
    new_listener->namespace = cat_strings(2, register_name, ".");
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
            if(strcmp(listener->namespace, plugin_name) == 0)
            {
                event_listener_free(listener);
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
        event_listener_free(listener);
        list_erase_node(event->listeners, node);
    }
}

void event_unregister_all_listeners_of_plugin(struct plugin_t* plugin)
{
    /* 
     * For every listener in every event, search for any listener that belongs
     * to the specified plugin
     */
    char* namespace = event_get_namespace_name(plugin);
    {
        LIST_FOR_EACH(&g_events, struct event_t, event)
        {
            event_unregister_all_listeners_of_namespace(event, namespace);
        }
    }
    FREE(namespace);
}

void event_dispatch(struct event_t* event, void* args)
{
    LIST_FOR_EACH(event->listeners, struct event_listener_t, listener)
    {
        listener->exec(event, args);
    }
}

static void event_unregister_all_listeners_of_namespace(struct event_t* event,
                                                        char* namespace)
{
    int len = strlen(namespace);
    {
        LIST_FOR_EACH_ERASE(event->listeners, struct event_listener_t, listener)
        {
            if(strncmp(listener->namespace, namespace, len) == 0)
            {
                event_listener_free(listener);
                list_erase_node(event->listeners, node);
            }
        }
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

static void event_free(struct event_t* event)
{
    event_unregister_all_listeners(event);
    FREE(event->name); /* full_name must be FREEd manually, see event_create() */
    list_destroy(event->listeners);
    FREE(event);
}

static void event_listener_free(struct event_listener_t* listener)
{
    FREE(listener->namespace);
    FREE(listener);
}
