#ifndef LIGHTSHIP_PLUGIN_MANAGER_H
#define LIGHTSHIP_PLUGIN_MANAGER_H

#include "plugin_manager/config.h"
#include "plugin_manager/plugin_api.h"

/*!
 * @brief Initialises the plugin manager.
 * 
 * Must be called before calling any other functions related to the plugin
 * manager.
 */
PLUGIN_MANAGER_PUBLIC_API void
plugin_manager_init(void);

PLUGIN_MANAGER_PUBLIC_API void
plugin_manager_get_services(struct game_t* game);

/*!
 * @brief Starts a loaded plugin.
 * 
 * This calls the plugin's plugin_start() function.
 * @param[in] plugin The plugin to start.
 * @return Returns 1 if successful, 0 if otherwise.
 */
#define plugin_start(game, plugin) ((plugin)->started_successfully = (plugin)->start(game))

#define plugin_stop(game, plugin) do { \
    if((plugin)->started_successfully) \
        ((plugin)->stop(game)); } while(0)

#define plugin_deinit(plugin) do { (plugin)->deinit(); } while(0)

/*!
 * @brief De-initialises the plugin manager.
 * 
 * This will unload all plugins cleanly and clean up any memory being used.
 */
PLUGIN_MANAGER_PUBLIC_API void
plugin_manager_deinit(struct game_t* game);

/*!
 * @brief Loads the specified plugin.
 * 
 * In order to load a plugin, one must provide the name of the plugin, the
 * desired version of the plugin, and search criteria. This is done by filling
 * out the datafields in a struct of type *plugin_info_t* and passing it to
 * this function. Example:
 * @code
 * plugin_info_t my_plugin;
 * my_plugin.name = "foo";
 * my_plugin.version.major = 1;
 * my_plugin.version.minor = 2;
 * my_plugin.version.patch = 6;
 * @endcode
 * Note that the name of the plugin isn't what the loaded plugin claims to be
 * named, but is a substring of the actual file name. If the name were
 * *foo*, then the file name would have to be *plugin_foo-1-2-6.so*.
 * 
 * The search criteria can be:
 *   - *PLUGIN_VERSION_EXACT*: The version of the file has to match exactly.
 *   - *PLUGIN_VERSION_MINIMUM*: The version of the file name must be at least
 *     equal or greater than the requested version.
 *
 * @param[in] plugin_info Requirements for the plugin to be loaded.
 * for a file matching the name in ./plugins, relative to the working
 * directory.
 * @param[in] criteria Set whether the desired plugin version should be exact
 * or if the desired plugin version should be greater or equal.
 * @return Returns NULL if unsuccessful, otherwise a pointer to the plugin is
 * returned.
 */
PLUGIN_MANAGER_PUBLIC_API struct plugin_t*
plugin_load(struct game_t* game,
            const struct plugin_info_t* plugin_info,
            plugin_search_criteria_t criteria);

PLUGIN_MANAGER_PUBLIC_API char
load_plugins_from_yaml(struct game_t* game, const char* filename);

/*!
 * @brief Unloads the specified plugin.
 * @param[in] plugin The plugin to unload.
 */
PLUGIN_MANAGER_PUBLIC_API void
plugin_unload(struct plugin_t* plugin);

/*!
 * @brief Searches all loaded plugins for a matching name.
 * @param[in] name The name of the plugin to search for.
 * @return Returns the plugin if a matching name was found. Returns NULL on
 * failure.
 */
PLUGIN_MANAGER_PUBLIC_API struct plugin_t*
plugin_get_by_name(struct game_t* game, const char* name);

#endif /* LIGHTSHIP_PLUGIN_MANAGER_H */
