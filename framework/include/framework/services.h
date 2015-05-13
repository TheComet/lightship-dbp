#ifndef FRAMEWORK_SERVICES_H
#define FRAMEWORK_SERVICES_H

#include "framework/service_api.h"

C_HEADER_BEGIN

struct plugin_t;
struct ordered_vector_t;

typedef enum service_script_type_e
{
    SERVICE_SCRIPT_TYPE_UNKNOWN,
    SERVICE_SCRIPT_TYPE_NONE,

    SERVICE_SCRIPT_TYPE_INT8,   /* NOTE: Interleaving signed and unsigned-ness so */
    SERVICE_SCRIPT_TYPE_UINT8,  /*       a signed type can be set to an unsigned */
    SERVICE_SCRIPT_TYPE_INT16,  /*       type, simply by adding 1. */
    SERVICE_SCRIPT_TYPE_UINT16,
    SERVICE_SCRIPT_TYPE_INT32,
    SERVICE_SCRIPT_TYPE_UINT32,
    SERVICE_SCRIPT_TYPE_INT64,
    SERVICE_SCRIPT_TYPE_UINT64,

    SERVICE_SCRIPT_TYPE_INTPTR,

    SERVICE_SCRIPT_TYPE_FLOAT,
    SERVICE_SCRIPT_TYPE_DOUBLE,

    SERVICE_SCRIPT_TYPE_STRING,
    SERVICE_SCRIPT_TYPE_WSTRING
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
 * @brief Registers a service to the global service directory.
 * @param[in] directory The name of the service. This should be unique within
 * the plugin, but can have the same name as other services in different
 * plugins.
 * @param[in] exec A function pointer to the service function.
 */
FRAMEWORK_PUBLIC_API struct service_t*
service_create(struct plugin_t* plugin,
               const char* directory,
               const service_callback_func exec,
               const char* ret_type,
               const int argc,
               const char** argv);

/*!
 * @brief Unregisters a service from the global service directory.
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

C_HEADER_END

#endif /* FRAMEWORK_SERVICES_H */
