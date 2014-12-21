#include "lightship/services.h"
#include "lightship/callback.h"
#include "util/plugin.h"

struct list_t g_services;

void services_init(void)
{
    list_init_list(&g_services);
}

char service_register(struct plugin_t* plugin,
                      const char* name,
                      intptr_t exec)
{
    return (callback_create_and_register(&g_services, plugin, name, exec) != NULL);
}

char service_unregister(struct plugin_t* plugin,
                        const char* name)
{
    return callback_destroy_and_unregister(&g_services, plugin, name);
}

void service_unregister_all(struct plugin_t* plugin)
{
    callback_destroy_all(&g_services, plugin);
}

intptr_t service_get(const char* name)
{
    return callback_get(&g_services, name);
}
