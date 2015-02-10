/*!
 * @file events.h 
 * @addtogroup events_and_services Events and Services
 * @page events Events Explained
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
evt_jump = event_create(plugin, "jump");
evt_run = event_create(plugin, "run");
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
 * macro EVENT_LISTENERn(), where n is the number of arguments being passed.
 *
 * First the callback function must be defined:
@code
EVENT_LISTENER0(on_player_jump)
{
    <do stuff to make a player jump>
}
@endcode
 * Next, the callback function needs to be registered to an event. It is very
 * important to **register listeners after PLUGIN_INIT()**. The reason for this
 * is because events are registered during PLUGIN_INIT() and may not be
 * available until after the plugin is fully initialised.
@code
<during or after PLUGIN_START()>
event_register_listener(plugin, "plugin_name.jump", on_player_jump);
@endcode
 * 
 * See event_register_listener() for more information.
 */

#ifndef LIGHTSHIP_UTIL_EVENTS_H
#define LIGHTSHIP_UTIL_EVENTS_H

#include "util/pstdint.h"
#include "util/linked_list.h"
#include "plugin_manager/config.h"
#include "plugin_manager/event_api.h"

C_HEADER_BEGIN

struct plugin_t;
struct log_t;
struct game_t;

/* ----------------------------
 * Built-in events
 * --------------------------*/
PLUGIN_MANAGER_PUBLIC_API EVENT_H1(evt_log, struct log_t*);
PLUGIN_MANAGER_PUBLIC_API EVENT_H1(evt_log_indent, const char*);
PLUGIN_MANAGER_PUBLIC_API EVENT_H0(evt_log_unindent);

/*!
 * @brief Initialises the event system.
 * @note Must be called before calling any other event related functions.
 */
PLUGIN_MANAGER_PUBLIC_API void
events_init(struct game_t* game);

/*!
 * @brief De-initialises the event system and cleans up any events that weren't
 * removed.
 */
PLUGIN_MANAGER_PUBLIC_API void
events_deinit(struct game_t* game);

/*!
 * @brief Creates and registers a new event in the host program.
 * 
 * @param[in] plugin The plugin object this event belongs to.
 * @param[in] name The name of the event. Should be unique plugin-wide.
 * @note The name string is copied to an internal buffer, so you are free to
 * delete it when it is no longer used.
 * @return Returns a new event object which should be stored by the plugin.
 */
PLUGIN_MANAGER_PUBLIC_API struct event_t*
event_create(struct game_t* game,
             const struct plugin_t* plugin,
             const char* name);

/*!
 * @brief Destroys an event object.
 * @note This also destroys all registered event listeners and removes it from
 * the assigned game object.
 * @param[in] event The event object to destroy.
 * @return Returns 1 if successful, 0 if otherwise.
 */
PLUGIN_MANAGER_PUBLIC_API char
event_destroy(struct event_t* event_delete);

/*!
 * @brief Destroys an event object by plugin object and name.
 * @note This also destroys all registered event listeners.
 * @param[in] plugin The plugin that created the event.
 * @param[in] name The name of the event.
 */
PLUGIN_MANAGER_PUBLIC_API void
event_destroy_plugin_event(struct game_t* game,
                           const struct plugin_t* plugin,
                           const char* name);

/*!
 * @brief Destroys all events that were registered by the specified plugin.
 * @note This also destroys all registered event listeners.
 * @param[in] plugin The plugin to destroy the events from.
 */
PLUGIN_MANAGER_PUBLIC_API void
event_destroy_all_plugin_events(const struct plugin_t* plugin);

/*!
 * @brief Returns an event object with the specified name.
 * @return If the event object does not exist, NULL is returned, otherwise the
 * event object is returned.
 */
PLUGIN_MANAGER_PUBLIC_API struct event_t*
event_get(const struct game_t* game, const char* full_name);

/*!
 * @brief Registers a listener to the specified event.
 */
PLUGIN_MANAGER_PUBLIC_API char
event_register_listener(const struct game_t* game,
                        const struct plugin_t* plugin,
                        const char* event_name,
                        event_callback_func callback);

/*!
 * @brief Unregisters a listener from the specified event.
 */
PLUGIN_MANAGER_PUBLIC_API char
event_unregister_listener(const struct game_t* game,
                          const char* plugin_name, 
                          const char* event_name);

/*!
 * @brief Unregisters all listeners from the specified event.
 */
PLUGIN_MANAGER_PUBLIC_API void
event_unregister_all_listeners(struct event_t* event);

/*!
 * @brief Unregisters all listeners that belong to the specified plugin
 * globally.
 * @param[in] plugin The plugin the listeners belong to.
 */
PLUGIN_MANAGER_PUBLIC_API void
event_unregister_all_listeners_of_plugin(const struct plugin_t* plugin);

C_HEADER_END

#endif /* LIGHTSHIP_UTIL_EVENTS_H */

/** @} */
