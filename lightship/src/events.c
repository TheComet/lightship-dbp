#include "lightship/events.h"
#include "lightship/callback.h"
#include "util/plugin.h"

struct list_t g_events;

void events_init(void)
{
    list_init_list(&g_events);
}

char event_register(struct plugin_t* plugin,
                      const char* name,
                      intptr_t exec)
{
    return callback_register(&g_events, plugin, name, exec);
}

char event_unregister(struct plugin_t* plugin,
                        const char* name)
{
    return callback_unregister(&g_events, plugin, name);
}

intptr_t event_get(const char* name)
{
    return callback_get(&g_events, name);
}
