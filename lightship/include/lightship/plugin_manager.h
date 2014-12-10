struct plugin_t;

/*!
 * @brief Initialises the plugin manager.
 * 
 * Must be called before calling any other functions related to the plugin
 * manager.
 */
void plugin_manager_init(void);

/*!
 * @brief Loads the specified plugin.
 * @param plugin_name The name of the plugin to load. The function will search
 * for a file matching the name in ./plugins, relative to the working
 * directory.
 * @return Returns NULL if unsuccessful, otherwise a pointer to the plugin is
 * returned.
 */
struct plugin_t* plugin_load(const char* plugin_name);

void plugin_unload(struct plugin_t* plugin);

void plugin_manager_deinit(void);