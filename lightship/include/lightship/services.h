#include "util/pstdint.h"
#include "util/linked_list.h"

struct plugin_t;

extern struct list_t g_services;

void services_init(void);
char service_register(struct plugin_t* plugin,
                      const char* name,
                      intptr_t exec);
char service_unregister(struct plugin_t* plugin,
                        const char* name);
void service_unregister_all(struct plugin_t* plugin);
intptr_t service_get(const char* name);
