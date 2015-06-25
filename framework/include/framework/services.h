#ifndef FRAMEWORK_SERVICES_H
#define FRAMEWORK_SERVICES_H

#include "framework/se_api.h"

C_HEADER_BEGIN

struct game_t;
struct ordered_vector_t;
struct plugin_t;

struct service_t
{
	struct plugin_t* plugin;    /* reference to the plugin that owns this service */
	char* directory;
	service_func exec;
	struct type_info_t* type_info;
};

/*!
 * @brief Initialises the service system. This must be called before calling any
 * other service related functions.
 */
FRAMEWORK_PUBLIC_API char
service_init(struct game_t* game);

/*!
 * @brief De-initialises the service system. This must be called to clean up
 * any memory allocated by the system before shutdown.
 */
FRAMEWORK_PUBLIC_API void
service_deinit(struct game_t* game);

/*!
 * @brief Registers a function as a service routine to the game's global
 * service directory.
 * @param[in] plugin The plugin to whom the function being registered belongs
 * to. The game object is taken from plugin->game, meaning whichever game
 * object was used to create the plugin will be used to hold the service.
 * @param[in] directory The name and directory under which to store the
 * new service. This should be unique globally within the context of the game
 * object. The recommended naming scheme is "plugin_name.service_name".
 * @param[in] exec A function pointer to the function.
 * TODO document rest after finishing se_api
 */
FRAMEWORK_PUBLIC_API struct service_t*
service_create(struct plugin_t* plugin,
			   const char* directory,
			   const service_func exec,
			   struct type_info_t* type_info);

/*!
 * @brief Unregisters a service from the global service directory and
 * de-allocates the service object.
 */
FRAMEWORK_PUBLIC_API void
service_destroy(struct service_t* service);

FRAMEWORK_PUBLIC_API uint32_t
service_destroy_all_matching(const char* pattern);

/*!
 * @brief Retrieves the specified service from the global service directory.
 * @param[in] name The global name, including namespace, of the service to get.
 * This follows the naming convention *plugin_name.service_name*.
 * @return The return value is an integer representing a function pointer to the
 * service. It must be cast to the exact function signature of the service
 * registered by the plugin. If the service does not exist, 0 is returned.
 */
FRAMEWORK_PUBLIC_API struct service_t*
service_get(struct game_t* game, const char* directory);

C_HEADER_END

#endif /* FRAMEWORK_SERVICES_H */
