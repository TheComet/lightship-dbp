#include "util/pstdint.h"
#include "util/linked_list.h"

struct plugin_t;

extern struct list_t g_events;

void events_init(void);
char event_register(struct plugin_t* plugin,
                      const char* name,
                      intptr_t exec);
char event_unregister(struct plugin_t* plugin,
                        const char* name);
void event_unregister_all(struct plugin_t* plugin);
intptr_t event_get(const char* name);
