#ifndef LIGHTSHIP_EVENTS_HPP
#define LIGHTSHIP_EVENTS_HPP

#include "util/pstdint.h"
#include "util/linked_list.h"
#include "util/event_api.h"

extern struct list_t g_events;

struct plugin_t;

/*!
 * @brief Initialises the event system.
 * 
 * Must be called before calling any other event related functions.
 */
void events_init(void);

/*!
 * @brief Creates and registers a new event in the host program.
 * 
 * 
 * @param plugin The plugin object this event belongs to.
 * @param name The name of the event. Should be unique plugin-wide.
 * @return Returns a new event object which should be stored by the plugin.
 */
struct event_t* event_create(struct plugin_t* plugin,
                             const char* name);

/*!
 * @brief Destroys an event object.
 * @note This also destroys all registered event listeners.
 * @param event The event object to destroy.
 * @return Returns 1 if successful, 0 if otherwise.
 */
char event_destroy(struct event_t* event_delete);

/*!
 * @brief Destroys an event object by plugin object and name.
 * @note This also destroys all registered event listeners.
 * @param plugin The plugin that created the event.
 * @param name The name of the event.
 */
void event_destroy_plugin_event(struct plugin_t* plugin,
                                const char* name);

/*!
 * @brief Destroys all events that were registered by the specified plugin.
 * @note This also destroys all registered event listeners.
 * @param plugin The plugin to destroy the events from.
 */
void event_destroy_all_plugin_events(struct plugin_t* plugin);

/*!
 * @brief Returns an event object with the specified name.
 * @return If the event object does not exist, NULL is returned, otherwise the
 * event object is returned.
 */
struct event_t* event_get(const char* full_name);

/*!
 * @brief Registers a listener to the specified event.
 */
char event_register_listener(struct plugin_t* plugin, const char* full_name, event_func callback);

/*!
 * @brief Unregisters a listener from the specified event.
 */
char event_unregister_listener(const char* event_name, const char* plugin_name);

/*!
 * @brief Unregisters all listeners from the specified event.
 */
void event_unregister_all_listeners(struct event_t* event);

/*!
 * @brief Dispatches an event with arguments to all listeners of the specified
 * event.
 */
void event_dispatch(struct event_t* event, void* args);

#endif /* LIGHTSHIP_EVENTS_HPP */
