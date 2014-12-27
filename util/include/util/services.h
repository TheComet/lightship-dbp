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
LIGHTSHIP_PUBLIC_API char service_register(struct plugin_t* plugin,
                      const char* name,
                      intptr_t exec);

/*!
 * @brief Allocates and registers a new service to the global service directory.
 * @note Internal function only. This is mainly used to register built-in
 * services.
 */
void service_malloc_and_register(char* full_name, intptr_t exec);

/*!
 * @brief Unregisters a service from the global service directory.
 * @param[in] plugin The plugin the service belongs to. The plugin name is used
 * to create the namespace under which the service is registered.
 * @param[in] name The name of the service to unregister.
 */
LIGHTSHIP_PUBLIC_API char service_unregister(struct plugin_t* plugin,
                        const char* name);

/*!
 * @brief Unregisters all services that were previously registered by the
 * specified plugin.
 * @param[in] plugin The plugin to unregister all services.
 */
LIGHTSHIP_PUBLIC_API void service_unregister_all(struct plugin_t* plugin);

/*!
 * @brief Retrieves the specified service from the global service directory.
 * @param[in] name The global name, including namespace, of the service to get.
 * This follows the naming convention *plugin_name.service_name*.
 * @return The return value is an integer representing a function pointer to the
 * service. It must be cast to the exact function signature of the service
 * registered by the plugin. If the service does not exist, 0 is returned.
 */
LIGHTSHIP_PUBLIC_API intptr_t service_get(const char* name);

#endif /* LIGHTSHIP_SERVICES_H */
