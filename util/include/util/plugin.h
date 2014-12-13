#include <util/plugin_api.h>

/*!
 * @brief Plugin object. For every loaded plugin there exists one instance of this.
 */
struct plugin_t
{
    struct plugin_info_t info;
    void* handle;
    plugin_start_func start;
    plugin_stop_func stop;
};

/*!
 * @brief Creates and initialises a new plugin object.
 * @note The plugin is not stored internally, it is expected for the
 * programmer to eventually call plugin_destroy() when the returned object is
 * no longer needed.
 * @return Returns the newly created plugin object.
 */
struct plugin_t* plugin_create(void);

/*!
 * @brief Initialises a plugin object.
 * @param plugin The plugin to initialise.
 */
void plugin_init_plugin(struct plugin_t* plugin);

/*!
 * @brief Destroys a plugin object.
 * @param plugin The plugin to destroy.
 */
void plugin_destroy(struct plugin_t* plugin);

/*!
 * @brief Sets general information about the plugin.
 * @param plugin The plugin to set information about.
 * @param name The name of the plugin. This should be globally unique and
 * should not change between versions, as other plugins might use this string
 * to discover your plugin.
 * @param category What category your plugin is in.
 * @param author The name of the plugin author(s).
 * @param description A short description about what your plugin does.
 * @param website A URL to your website.
 */
void plugin_set_info(struct plugin_t* plugin,
                     const char* name,
                     const char* category,
                     const char* author,
                     const char* description,
                     const char* website);

/*!
 * @brief Frees all buffers allocated for info strings.
 */
static void plugin_free_info(struct plugin_t* plugin);

/*!
 * @brief Sets the programming language of the plugin.
 * @param plugin The plugin to set.
 * @param language The programming language.
 */
void plugin_set_programming_language(struct plugin_t* plugin, plugin_programming_language_t language);

/*!
 * @brief Sets the version of the plugin.
 * @param plugin The plugin to set.
 * @param major The major version.
 * @param minor The minor version.
 * @param patch The patch version.
 */
void plugin_set_version(struct plugin_t* plugin, uint32_t major, uint32_t minior, uint32_t patch);

/* TODO plugin dependencies */
void plugin_add_dependency(struct plugin_info_t* plugin);

/*!
 * @brief Extracts the three version digits from a string.
 * @param [in] file The file name to extract them from.
 * @param [out] major Where to store the major number.
 * @param [out] minor Where to sotre the minor number.
 * @param [out] patch Where to store the patch number.
 * @return Returns 1 if successful, 0 if otherwise.
 */
int plugin_extract_version_from_string(const char* file,
                                       uint32_t* major,
                                       uint32_t* minor,
                                       uint32_t* patch);

/*!
 * @brief Generates a version string from the info struct and copies it into
 * the provided buffer.
 * @param [out] str The buffer to copy the version string into.
 * @note The buffer should be at least sizeof(int)*27+1.
 * @param info The plugin info struct from which to generate the version
 * string.
 */
void plugin_get_version_string(char* str, struct plugin_info_t* info);
