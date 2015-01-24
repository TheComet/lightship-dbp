#ifndef LIGHTSHIP_SERVICES_H
#define LIGHTSHIP_SERVICES_H

#include "util/service_api.h"

C_HEADER_BEGIN

struct plugin_t;

LIGHTSHIP_PUBLIC_API char
service_register_(const struct plugin_t* plugin,
                  const char* name,
                  const intptr_t exec,
                  const char* ret_type,
                  const int argc,
                  const char** argv);

/*!
 * @brief Initialises the service system. This must be called before calling any
 * other service related functions.
 */
LIGHTSHIP_PUBLIC_API void
services_init(void);

/*!
 * @brief De-initialises the service system. This must be called to clean up
 * any memory allocated by the system before shutdown.
 */
LIGHTSHIP_PUBLIC_API void
services_deinit(void);

/*!
 * @brief Registers a service to the global service directory.
 * @param[in] plugin The plugin the service belongs to. The plugin name is used
 * to create the namespace under which the service name is registered.
 * @param[in] name The name of the service. This should be unique within the
 * plugin, but can have the same name as other services in different plugins.
 * @param[in] exec A function pointer to the service function.
 */
LIGHTSHIP_PUBLIC_API char
service_register(const struct plugin_t* plugin,
                 const char* name,
                 intptr_t exec);

/*!
 * @brief Frees the specified service object's contents and object itself.
 * @param service The service object to free
 */
LIGHTSHIP_PUBLIC_API void
service_free(struct service_t* service);

/*!
 * @brief Unregisters a service from the global service directory.
 * @param[in] plugin The plugin the service belongs to. The plugin name is used
 * to create the namespace under which the service is registered.
 * @param[in] name The name of the service to unregister.
 */
LIGHTSHIP_PUBLIC_API char
service_unregister(const struct plugin_t* plugin,
                   const char* name);

/*!
 * @brief Unregisters all services that were previously registered by the
 * specified plugin.
 * @param[in] plugin The plugin to unregister all services.
 */
LIGHTSHIP_PUBLIC_API void
service_unregister_all(const struct plugin_t* plugin);

/*!
 * @brief Retrieves the specified service from the global service directory.
 * @param[in] name The global name, including namespace, of the service to get.
 * This follows the naming convention *plugin_name.service_name*.
 * @return The return value is an integer representing a function pointer to the
 * service. It must be cast to the exact function signature of the service
 * registered by the plugin. If the service does not exist, 0 is returned.
 */
LIGHTSHIP_PUBLIC_API intptr_t
service_get(const char* name);

LIGHTSHIP_PUBLIC_API intptr_t
service_get_with_typecheck(const char* name, const char* ret_type, int argc, const char** argv);

LIGHTSHIP_PUBLIC_API void
service_auto_call_void(const char* name, int argc, const char** argv);

LIGHTSHIP_PUBLIC_API float
service_auto_call_float(const char* name, int argc, const char** argv);

LIGHTSHIP_PUBLIC_API uint32_t
service_auto_call_int(const char* name, int argc, const char** argv);

C_HEADER_END

#endif /* LIGHTSHIP_SERVICES_H */
