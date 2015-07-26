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
 * ```
 * EVENT_H(evt_jump)
 * EVENT_H(evt_run)
 * ```
 *
 * **events.c:**
 * ```
 * EVENT_C(evt_jump)
 * EVENT_C(evt_run)
 * ```
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
 * ```
 * (during PLUGIN_INIT())
 * evt_jump = event_create(plugin, "jump");
 * evt_run = event_create(plugin, "run");
 * ```
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
 * ```
 * EVENT_LISTENER0(on_player_jump)
 * {
 *     (do stuff to make a player jump)
 * }
 * ```
 * Next, the callback function needs to be registered to an event. It is very
 * important to **register listeners after PLUGIN_INIT()**. The reason for this
 * is because events are registered during PLUGIN_INIT() and may not be
 * available until after the plugin is fully initialised.
 * ```
 * (during or after PLUGIN_START())
 * event_register_listener(plugin, "plugin_name.jump", on_player_jump);
 * ```
 *
 * See event_register_listener() for more information.
 */

#ifndef FRAMEWORK_EVENTS_H
#define FRAMEWORK_EVENTS_H

#include "util/pstdint.h"
#include "util/linked_list.h"
#include "framework/config.h"
#include "framework/se_api.h"

C_HEADER_BEGIN

struct plugin_t;
struct log_t;
struct game_t;

struct event_t
{
	struct plugin_t* plugin;    /* reference to the plugin object that owns this event */
	char* directory;
	struct type_info_t* type_info;
	struct unordered_vector_t listeners; /* holds event_listener_t objects */
};

struct event_listener_t
{
	event_callback_func exec;
};

/*!
 * @brief Initialises the event system.
 * @note Must be called before calling any other event related functions.
 */
char
events_init(struct game_t* game);

void
events_deinit(struct game_t* game);

/*!
 * @brief Creates and registers a new event in the host program.
 *
 * @param[in] plugin The plugin object this event belongs to.
 * @param[in] directory The name of the event. Should be unique plugin-wide.
 * @note The name string is copied to an internal buffer, so you are free to
 * delete it when it is no longer used.
 * @return Returns a new event object which should be stored by the plugin.
 */
FRAMEWORK_PUBLIC_API struct event_t*
event_create(struct plugin_t* plugin,
			 const char* directory,
			 struct type_info_t* type_info);

/*!
 * @brief Destroys an event object.
 * @note This also destroys all registered event listeners and removes it from
 * the assigned game object.
 * @param[in] event The event object to destroy.
 * @return Returns 1 if successful, 0 if otherwise.
 */
FRAMEWORK_PUBLIC_API void
event_destroy(struct event_t* event);

/*!
 * @brief Destroys all events that were registered by the specified plugin.
 * @note This also destroys all registered event listeners.
 * @param[in] plugin The plugin to destroy the events from.
 */
/* TODO implement */
FRAMEWORK_PUBLIC_API void
event_destroy_all_matching(const char* pattern);

/*!
 * @brief Returns an event object with the specified name.
 * @return If the event object does not exist, NULL is returned, otherwise the
 * event object is returned.
 */
FRAMEWORK_PUBLIC_API struct event_t*
event_get(const struct game_t* game, const char* directory);

/*!
 * @brief Registers a listener to the specified event.
 * @note The same callback function will not be registered twice.
 * @param[in] game The game hosting the event you want to listen to.
 * @param[in] event_name The name of the event to register to.
 * @param[in] callback The callback function to call when the event is fired.
 */
FRAMEWORK_PUBLIC_API char
event_register_listener(const struct game_t* game,
						const char* event_directory,
						event_callback_func callback);

/*!
 * @brief Unregisters a listener from the specified event.
 */
FRAMEWORK_PUBLIC_API char
event_unregister_listener(const struct game_t* game,
						  const char* event_directory,
						  event_callback_func callback);

/*!
 * @brief Unregisters all listeners from the specified event.
 */
FRAMEWORK_PUBLIC_API void
event_unregister_all_listeners(struct event_t* event);

C_HEADER_END

#endif /* FRAMEWORK_EVENTS_H */

/** @} */
