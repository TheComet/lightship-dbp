#ifndef FRAMEWORK_SERVICES_H
#define FRAMEWORK_SERVICES_H

#include "framework/se_api.h"

C_HEADER_BEGIN

struct plugin_t;
struct ordered_vector_t;

typedef enum service_script_type_e
{
    SERVICE_TYPE_UNKNOWN,
    SERVICE_TYPE_NONE,

    SERVICE_TYPE_INT8,   /* NOTE: Interleaving signed and unsigned-ness so */
    SERVICE_TYPE_UINT8,  /*       a signed type can be set to an unsigned */
    SERVICE_TYPE_INT16,  /*       type, simply by adding 1. */
    SERVICE_TYPE_UINT16,
    SERVICE_TYPE_INT32,
    SERVICE_TYPE_UINT32,
    SERVICE_TYPE_INT64,
    SERVICE_TYPE_UINT64,
    SERVICE_TYPE_INTPTR,
    SERVICE_TYPE_UINTPTR,

    SERVICE_TYPE_FLOAT,
    SERVICE_TYPE_DOUBLE,

    SERVICE_TYPE_STRING,
    SERVICE_TYPE_WSTRING
} service_script_type_e;

/*!
 * @brief Initialises the service system. This must be called before calling any
 * other service related functions.
 */
FRAMEWORK_PUBLIC_API char
services_register_core_services(struct game_t* game);

/*!
 * @brief De-initialises the service system. This must be called to clean up
 * any memory allocated by the system before shutdown.
 */
FRAMEWORK_PUBLIC_API void
services_deinit(struct game_t* game);

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
               const char* ret_type,
               const int argc,
               const char** argv);

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

FRAMEWORK_PUBLIC_API char
service_do_typecheck(const struct service_t* service, const char* ret_type, uint32_t argc, const char** argv);

FRAMEWORK_PUBLIC_API void**
service_create_argument_list_from_strings(struct service_t* service, struct ordered_vector_t* argv);

FRAMEWORK_PUBLIC_API void
service_destroy_argument_list(struct service_t* service, void** argv);

service_script_type_e
service_get_type_from_string(const char* type);

C_HEADER_END

#endif /* FRAMEWORK_SERVICES_H */
