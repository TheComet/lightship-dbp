/*!
 * @file events.h 
 * @addtogroup events
 * @brief Communication within and between plugins.
 * @{
 * 
 * Basic Idea
 * ----------
 * Events are the main mechanism for a plugin to communicate with other
 * plugins. Actually, it is the main mechanism for communication within
 * plugins, too. You can think of an event as a cry for attention on the
 * plugin's behalf when it triggers. In the vast sea of code, it says *Hey!
 * I'm doing X just in case anyone is interested!* Whether or not something
 * else actually hears the cry is another matter, but the important thing is
 * you got the word out.
 * 
 * Events are registered in a global directory inside the host program using
 * the plugin name as a namespace for events it registers. So if a plugin with
 * the name **foo** were to register an event with the name **bar**, then the
 * event would be globally known as **foo.bar**.
 * 
 * Defining Events
 * ------------------
 * Every action that should be acted upon can and should be declared as an
 * event.
 * 
 * Events are statically defined in the **events.c** and **events.h** files.
 * Because your events are going to be used in other source files, you have to
 * define each event once in a source file and again in a header file, but
 * extern.
 * 
 * For instance, if your plugin has an event that should make the player jump
 * and another event that should make the player run, they are defined as:
 * 
 * **events.h:**
@code
EVENT_H(evt_jump)
EVENT_H(evt_run)
@endcode
 * 
 * **events.c:**
@code
EVENT_C(evt_jump)
EVENT_C(evt_run)
@endcode
 * 
 * **EVENT_H** is a helper macro to declare an extern variable for the
 * specified event. **EVENT_C** is a helper macro for defining and initialising
 * the same variable.
 * 
 * Registering Events
 * ------------------
 * Now that the events are defined they must be registered with the host
 * program. It is very important to **register all events during PLUGIN_INIT()**.
 * The reason for this is that other plugins will be looking for your events
 * immediately after your plugin is initialised.
 * 
 * Events are registered by calling event_create(). The return struct should be
 * stored in the event variables defined earlier. Again, the sample with run
 * and jump:
@code
<during PLUGIN_INIT()>
evt_jump = api->create_event(plugin, "jump");
evt_run = api->create_event(plugin, "run");
@endcode
 *
 * Once that is done, every plugin - including the one that registered the
 * events - will be able to listen to them under the name **plugin.jump** and
 * **plugin.run**, where **plugin** is the name of the plugin that registered
 * the event.
 *
 * Listening to Events
 * -------------------
 * Any registered event can be listened to by anyone. The listener callback
 * function has the same signature for every event, and is defined using the
 * macro EVENT_LISTENER(). 
 *
 * First the callback function must be defined:
@code
EVENT_LISTENER(on_player_jump)
{
    <do stuff to make a player jump>
}
@endcode
 * Next, the callback function needs to be registered to an event. It is very
 * important to **register listeners after PLUGIN_INIT()**. The reason for this
 * is because events are registered during PLUGIN_INIT() and may not be
 * available until after the plugin is fully initialised.
@code
api->event_register_listener(plugin, "plugin_name.jump", on_player_jump);
@endcode
 * 
 * See event_register_listener() for more information.
 */

#ifndef LIGHTSHIP_EVENTS_HPP
#define LIGHTSHIP_EVENTS_HPP

#include "util/pstdint.h"
#include "util/linked_list.h"
#include "util/event_api.h"

extern struct list_t g_events;

/* ----------------------------
 * Built-in events
 * --------------------------*/
EVENT_H(evt_log);

struct plugin_t;

/*!
 * @brief Initialises the event system.
 * @note Must be called before calling any other event related functions.
 */
void events_init(void);

/*!
 * @brief De-initialises the event system and cleans up any events that weren't
 * removed.
 */
void events_deinit(void);

/*!
 * @brief Creates and registers a new event in the host program.
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
 * @brief Unregisters all listeners that belong to the specified plugin
 * globally.
 * @param plugin The plugin the listeners belong to.
 */
void event_unregister_all_listeners_of_plugin(struct plugin_t* plugin);

/*!
 * @brief Dispatches an event with arguments to all listeners of the specified
 * event.
 */
void event_dispatch(struct event_t* event, void* args);

#endif /* LIGHTSHIP_EVENTS_HPP */

/** @} */
