#ifndef LIGHTSHIP_UTIL_PLUGIN_HPP
#define LIGHTSHIP_UTIL_PLUGIN_HPP

#include "util/plugin_api.h"
#include "util/config.h"

/*!
 * @brief Creates and initialises a new plugin object.
 * @note The plugin is not stored internally, it is expected for the
 * programmer to eventually call plugin_destroy() when the returned object is
 * no longer needed.
 * @return Returns the newly created plugin object.
 */
LIGHTSHIP_PUBLIC_API struct plugin_t* plugin_create(void);

/*!
 * @brief Initialises a plugin object.
 * @param [in] plugin The plugin to initialise.
 */
LIGHTSHIP_PUBLIC_API void plugin_init_plugin(struct plugin_t* plugin);

/*!
 * @brief Destroys a plugin object.
 * @param [in] plugin The plugin to destroy.
 */
LIGHTSHIP_PUBLIC_API void plugin_destroy(struct plugin_t* plugin);

/*!
 * @brief Sets general information about the plugin.
 * @param [in] plugin The plugin to set information about.
 * @param [in] name The name of the plugin. This should be globally unique and
 * should not change between versions, as other plugins might use this string
 * to discover your plugin.
 * @param [in] category What category your plugin is in.
 * @param [in] author The name of the plugin author(s).
 * @param [in] description A short description about what your plugin does.
 * @param [in] website A URL to your website.
 */
LIGHTSHIP_PUBLIC_API void plugin_set_info(struct plugin_t* plugin,
                     const char* name,
                     const char* category,
                     const char* author,
                     const char* description,
                     const char* website);

/*!
 * @brief Sets the programming language of the plugin.
 * @param [in] plugin The plugin to set.
 * @param [in] language The programming language.
 */
LIGHTSHIP_PUBLIC_API void plugin_set_programming_language(struct plugin_t* plugin,
                                     plugin_programming_language_t language);

/*!
 * @brief Sets the version of the plugin.
 * @param [in] plugin The plugin to set.
 * @param [in] major The major version.
 * @param [in] minor The minor version.
 * @param [in] patch The patch version.
 */
LIGHTSHIP_PUBLIC_API void plugin_set_version(struct plugin_t* plugin, uint32_t major, uint32_t minior, uint32_t patch);

/* TODO plugin dependencies */
LIGHTSHIP_PUBLIC_API void plugin_add_dependency(struct plugin_info_t* plugin);

/*!
 * @brief Extracts the three version digits from a string.
 * @param [in] file The file name to extract them from.
 * @param [out] major Where to store the major number.
 * @param [out] minor Where to sotre the minor number.
 * @param [out] patch Where to store the patch number.
 * @return Returns 1 if successful, 0 if otherwise.
 */
LIGHTSHIP_PUBLIC_API int plugin_extract_version_from_string(const char* file,
                                       uint32_t* major,
                                       uint32_t* minor,
                                       uint32_t* patch);

/*!
 * @brief Generates a version string from the info struct and copies it into
 * the provided buffer.
 * @param [out] str The buffer to copy the version string into.
 * @note The buffer should be at least sizeof(int)*27+1.
 * @param [in] info The plugin info struct from which to generate the version
 * string.
 */
LIGHTSHIP_PUBLIC_API void plugin_get_version_string(char* str, struct plugin_info_t* info);

#endif /* LIGHTSHIP_UTIL_PLUGIN_HPP */
