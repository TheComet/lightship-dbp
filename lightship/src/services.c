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
    return callback_register(&g_services, plugin, name, exec);
}

char service_unregister(struct plugin_t* plugin,
                        const char* name)
{
    return callback_unregister(&g_services, plugin, name);
}

void service_unregister_all(struct plugin_t* plugin)
{
    callback_unregister_all(&g_services, plugin);
}

intptr_t service_get(const char* name)
{
    return callback_get(&g_services, name);
}
