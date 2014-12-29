#ifndef LIGHTSHIP_SERVICES_H
#define LIGHTSHIP_SERVICES_H

#include "util/pstdint.h"
#include "util/linked_list.h"

struct plugin_t;

struct service_t
{
    char* name;
    intptr_t exec;
};

extern struct list_t g_services;

/*!
 * @brief Initialises the service system. This must be called before calling any
 * other service related functions.
 */
LIGHTSHIP_PUBLIC_API void services_init(void);

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
 * @brief Allocates and registers a new service. This is for internal use.
 * @param[in] full_name The full name, including namespace, of the service to
 * create and register.
 * @note *full_name* is owned by the service object after calling this function
 * and will be freed automatically. Therefore, you should pass a malloc'd string
 * as a parameter.
 * @param exec The function address of the callback function of the service.
 */
void
service_malloc_and_register(char* full_name, const intptr_t exec);

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

#endif /* LIGHTSHIP_SERVICES_H */
