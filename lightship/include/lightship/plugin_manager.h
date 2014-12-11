#include <util/pstdint.h>

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

/*!
 * @brief De-initialises the plugin manager.
 * 
 * This will unload all plugins cleanly and clean up any memory being used.
 */
void plugin_manager_deinit(void);

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

/*!
 * @brief Unloads the specified plugin.
 * @param plugin The plugin to unload.
 */
void plugin_unload(struct plugin_t* plugin);

/*!
 * @brief Searches all loaded plugins for a matching name.
 * @param name The name of the plugin to search for.
 * @return Returns the plugin if a matching name was found. Returns NULL on
 * failure.
 */
struct plugin_t* plugin_get_by_name(const char* name);

/*!
 * @brief Extracts the three version digits from a string.
 * @param [in] file The file name to extract them from.
 * @param [out] major Where to store the major number.
 * @param [out] minor Where to sotre the minor number.
 * @param [out] patch Where to store the patch number.
 * @return Returns 1 if successful, 0 if otherwise.
 */
static int plugin_extract_version_from_string(const char* file,
                                       uint32_t* major,
                                       uint32_t* minor,
                                       uint32_t* patch);

/*!
 * @brief Evaluates whether the specified file is an acceptable plugin to load
 * based on the specified info and criteria.
 * @param info The requested plugin to try and match.
 * @param file The file to test.
 * @param criteria The criteria to use.
 * @return Returns 1 if successful, 0 if otherwise.
 */
static int plugin_version_acceptable(struct plugin_info_t* info,
                        const char* file,
                        plugin_search_criteria_t criteria);

/*!
 * @brief Scans the plugin directory for a suitable plugin to load.
 * @param info The requested plugin to try and match.
 * @param criteria The criteria to use.
 * @return Returns the full file name and relative path if a plugin was
 * matched. Returns NULL on failure.
 */
static char* find_plugin(struct plugin_info_t* info,
                         plugin_search_criteria_t criteria);