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

void services_init(void);
LIGHTSHIP_PUBLIC_API char service_register(struct plugin_t* plugin,
                      const char* name,
                      intptr_t exec);
void service_malloc_and_register(char* full_name, intptr_t exec);
LIGHTSHIP_PUBLIC_API char service_unregister(struct plugin_t* plugin,
                        const char* name);
LIGHTSHIP_PUBLIC_API void service_unregister_all(struct plugin_t* plugin);
LIGHTSHIP_PUBLIC_API intptr_t service_get(const char* name);

#endif /* LIGHTSHIP_SERVICES_H */
