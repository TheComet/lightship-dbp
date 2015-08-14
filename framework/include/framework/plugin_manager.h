#ifndef LIGHTSHIP_PLUGIN_MANAGER_H
#define LIGHTSHIP_PLUGIN_MANAGER_H

#include "framework/config.h"
#include "framework/plugin_api.h"

struct ptree_t;

/*!
 * @brief Calls the plugin's PLUGIN_START() function.
 * @param[in] plugin The plugin to start.
 * @return Returns 1 if successful, 0 if otherwise.
 */
#define plugin_start(game, plugin) ((plugin)->started_successfully = (plugin)->start(game))

/*!
 * @brief Attempts to call the plugin's PLUGIN_STOP() function.
 * @note If this is called on a plugin that didn't return PLUGIN_SUCCESS on
 * plugin_start(), then this function has no effect. Plugin authors need to
 * keep this in mind so they can avoid memory leaks.
 */
#define plugin_stop(plugin) do { \
	if((plugin)->started_successfully) \
		((plugin)->stop((plugin)->game)); } while(0)

/*!
 * @brief Calls the plugin's PLUGIN_DEINIT() function.
 */
#define plugin_deinit(plugin) do { (plugin)->deinit((plugin)->game); } while(0)

/*!
 * @brief Initialises the plugin manager for the specified game object.
 *
 * Each game object can have one plugin manager.
 * @param[in] game The game to initialise for.
 */
FRAMEWORK_PUBLIC_API char
plugin_manager_init(struct game_t* game);

/*!
 * @brief De-initialises the plugin manager.
 *
 * This will unload all plugins cleanly and clean up any memory being used.
 */
FRAMEWORK_PUBLIC_API void
plugin_manager_deinit(struct game_t* game);

/*!
 * @brief Loads the specified plugin.
 *
 * In order to load a plugin, one must provide the name of the plugin, the
 * desired version of the plugin, and search criteria. This is done by filling
 * out the datafields in a struct of type *plugin_info_t* and passing it to
 * this function. Example:
 * ```
 * plugin_info_t my_plugin;
 * my_plugin.name = "foo";
 * my_plugin.version.major = 1;
 * my_plugin.version.minor = 2;
 * my_plugin.version.patch = 6;
 * ```
 * Note that the name of the plugin isn't what the loaded plugin claims to be
 * named, but is a substring of the actual file name. If the name were
 * *foo*, then the file name would have to be *plugin_foo-1-2-6.so*.
 *
 * The search criteria can be:
 *   - *PLUGIN_VERSION_EXACT*: The version of the file has to match exactly.
 *   - *PLUGIN_VERSION_MINIMUM*: The version of the file name must be at least
 *     equal or greater than the requested version.
 *
 * This function will call the plugin's PLUGIN_INIT() function but will **not**
 * call PLUGIN_START(). This must be done manually with the returned plugin by
 * using:
 * ```
 * plugin = plugin_load(game, info, PLUGIN_VERSION_EXACT);
 * plugin_start(game, plugin);
 * ```
 *
 * @note If plugin_start() returns PLUGIN_FAILURE, then the plugin's
 * PLUGIN_STOP() will never be called, even if you use plugin_stop().
 *
 * @param[in] plugin_info Requirements for the plugin to be loaded.
 * for a file matching the name in ./plugins, relative to the working
 * directory.
 * @param[in] criteria Set whether the desired plugin version should be exact
 * or if the desired plugin version should be greater or equal.
 * @return Returns NULL if unsuccessful, otherwise a pointer to the plugin is
 * returned.
 */
FRAMEWORK_PUBLIC_API struct plugin_t*
plugin_load(struct game_t* game,
			const struct plugin_info_t* plugin_info,
			plugin_search_criteria_e criteria);

FRAMEWORK_PUBLIC_API char
load_plugins_from_yaml(struct game_t* game, const struct ptree_t* plugins_node);

/*!
 * @brief Unloads the specified plugin.
 * @param[in] plugin The plugin to unload.
 */
FRAMEWORK_PUBLIC_API void
plugin_unload(struct game_t* game, struct plugin_t* plugin);

/*!
 * @brief Searches all loaded plugins for a matching name.
 * @param[in] name The name of the plugin to search for.
 * @return Returns the plugin if a matching name was found. Returns NULL on
 * failure.
 */
FRAMEWORK_PUBLIC_API struct plugin_t*
plugin_get_by_name(struct game_t* game, const char* name);

#endif /* LIGHTSHIP_PLUGIN_MANAGER_H */
