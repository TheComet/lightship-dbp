#include "util/services.h"
#include "util/memory.h"
#include "util/plugin.h"
#include "util/string.h"
#include "util/map.h"
#include "util/hash.h"
#include <stdlib.h>
#include <string.h>

struct map_t g_services;

/* ------------------------------------------------------------------------- */
void
services_init(void)
{
    map_init_map(&g_services);
    
    /* ----------------------------
     * Register built-in services 
     * --------------------------*/
    
    
}

/* ------------------------------------------------------------------------- */
void
services_deinit(void)
{
    MAP_FOR_EACH(&g_services, struct service_t, key, service)
    {
        service_free(service);
    }
    map_clear_free(&g_services);
}

/* ------------------------------------------------------------------------- */
char
service_register(const struct plugin_t* plugin,
                 const char* name,
                 intptr_t exec)
{
    char* full_name;

    /* check if service is already registered */
    full_name = cat_strings(3, plugin->info.name, ".", name);
    if(service_get(full_name))
    {
        free_string(full_name);
        return 0;
    }

    service_malloc_and_register(full_name, exec);

    return 1;
}

/* ------------------------------------------------------------------------- */
void
service_malloc_and_register(char* full_name, const intptr_t exec)
{
    /* create service and add to list */
    struct service_t* service = (struct service_t*)MALLOC(sizeof(struct service_t));
    service->name = full_name;
    service->exec = exec;
    map_insert(&g_services, hash_jenkins_oaat(full_name, strlen(full_name)), service);
}

/* ------------------------------------------------------------------------- */
void
service_free(struct service_t* service)
{
    free_string(service->name);
    FREE(service);
}

/* ------------------------------------------------------------------------- */
char
service_unregister(const struct plugin_t* plugin,
                   const char* name)
{
    char* full_name;
    intptr_t hash;
    struct service_t* service;

    /* remove service from map */
    full_name = cat_strings(3, plugin->info.name, ".", name);
    hash = hash_jenkins_oaat(full_name, strlen(full_name));
    free_string(full_name);
    if(!(service = map_erase(&g_services, hash)))
        return 0;
    service_free(service);
    return 1;
}

/* ------------------------------------------------------------------------- */
void
service_unregister_all(const struct plugin_t* plugin)
{
    char* name = cat_strings(2, plugin->info.name, ".");
    int len = strlen(plugin->info.name);
    {
        MAP_FOR_EACH(&g_services, struct service_t, key, service)
        {
            if(strncmp(service->name, name, len) == 0)
            {
                service_free(service);
                MAP_ERASE_CURRENT_ITEM_IN_FOR_LOOP(&g_services);
            }
        }
    }
    free_string(name);
}

/* ------------------------------------------------------------------------- */
intptr_t
service_get(const char* name)
{
    struct service_t* service;
    if(!(service = map_find(&g_services, hash_jenkins_oaat(name, strlen(name)))))
        return 0;
    return service->exec;
}
