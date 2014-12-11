struct plugin_t;
struct plugin_info_t;

typedef enum plugin_search_criteria_t
{
    PLUGIN_VERSION_EXACT,
    PLUGIN_VERSION_MINIMUM
} plugin_search_criteria_t;

/*!
 * @brief Initialises the plugin manager.
 * 
 * Must be called before calling any other functions related to the plugin
 * manager.
 */
void plugin_manager_init(void);

void plugin_manager_deinit(void);

/*!
 * @brief Loads the specified plugin.
 * @param plugin_info Requirements for the plugin to be loaded.
 * for a file matching the name in ./plugins, relative to the working
 * directory.
 * @param criteria Set whether the desired plugin version should be exact or if
 * the desired plugin version should be greater or equal.
 * @return Returns NULL if unsuccessful, otherwise a pointer to the plugin is
 * returned.
 */
struct plugin_t* plugin_load(struct plugin_info_t* plugin_info,
                             plugin_search_criteria_t criteria);

void plugin_unload(struct plugin_t* plugin);

struct plugin_t* plugin_get_by_name(const char* name);