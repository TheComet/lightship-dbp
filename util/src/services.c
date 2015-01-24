#include "util/services.h"
#include "util/memory.h"
#include "util/plugin.h"
#include "util/string.h"
#include "util/map.h"
#include "util/hash.h"
#include <stdlib.h>
#include <string.h>

struct map_t g_services;

/*!
 * @brief Allocates and registers a new service. This is for internal use.
 * @param[in] full_name The full name, including namespace, of the service to
 * create and register.
 * @note *full_name* is owned by the service object after calling this function
 * and will be freed automatically. Therefore, you should pass a malloc'd string
 * as a parameter.
 * @param exec The function address of the callback function of the service.
 */
static void
service_malloc_and_register(char* full_name, const intptr_t exec);

static void
service_malloc_and_register_(char* full_name,
                             const intptr_t exec,
                             const char* ret_type,
                             const int argc,
                             const char** argv);

static char
service_type_matches_simple_type(const char* type, const char* script_type);

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

char
service_register_(const struct plugin_t* plugin,
                  const char* name,
                  const intptr_t exec,
                  const char* ret_type,
                  const int argc,
                  const char** argv)
{
    char* full_name;

    /* check if service is already registered */
    full_name = cat_strings(3, plugin->info.name, ".", name);
    if(service_get(full_name))
    {
        free_string(full_name);
        return 0;
    }

    service_malloc_and_register_(full_name, exec, ret_type, argc, argv);

    return 1;
}

/* ------------------------------------------------------------------------- */
static void
service_malloc_and_register(char* full_name, const intptr_t exec)
{
    /* create service and add to list */
    struct service_t* service = (struct service_t*)MALLOC(sizeof(struct service_t));
    service->name = full_name;
    service->exec = exec;
    map_insert(&g_services, hash_jenkins_oaat(full_name, strlen(full_name)), service);
}

static void
service_malloc_and_register_(char* full_name,
                             const intptr_t exec,
                             const char* ret_type,
                             const int argc,
                             const char** argv)
{
    /* create service and add to list */
    struct service_t* service = (struct service_t*)MALLOC(sizeof(struct service_t));
    service->name = full_name;
    memcpy(&service->cb.ret_type, &ret_type, sizeof(char*));
    memcpy(&service->cb.argv, &argv, sizeof(char**));
    service->cb.argc = argc;
    service->cb.exec = exec;
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

intptr_t
service_get_with_typecheck(const char* name, const char* ret_type, int argc, const char** argv)
{
    int i;
    struct service_t* service;
    if(!(service = map_find(&g_services, hash_jenkins_oaat(name, strlen(name)))))
        return 0;

    /* verify argument count */
    if(argc != service->cb.argc)
        return 0;
    /* verify return type */
    if(!ret_type || strcmp(ret_type, service->cb.ret_type))
        return 0;
    /* verify argument types */
    for(i = 0; i != argc; ++i)
        if(!argv[i] || strcmp(argv[i], service->cb.argv[i]))
            return 0;
    
    /* valid! */
    return service->cb.exec;
}

void
service_auto_call_void(const char* name, int argc, const char** argv)
{
    int i;
    struct service_t* service;
    if(!(service = map_find(&g_services, hash_jenkins_oaat(name, strlen(name)))))
        return 0;
    
    /* verify argument count */
    if(argc != service->cb.argc)
        return 0;
    /* verify return type */
    if(!service_type_matches_simple_type(service->cb.ret_type, "void"))
        return 0;
    /* verify argument types */
    for(i = 0; i != argc; ++i)
        if(!argv[i] || !service_type_matches_simple_type(service->cb.argv[i], argv[i]))
            return 0;
    
    /* valid! -- */
    return service->cb.exec;
}

static char
service_type_matches_simple_type(const char* type, const char* script_type)
{
    /* strings */
    if(!strcmp(script_type, "string"))
    {
        /* any form of char* is acceptable */
        if(!strstr(type, "char*"))
            return 0;
        /* make sure it's not actually a char** */
        if(strstr(type, "**"))
            return 0;
        return 1;
    }
    
    /* integers */
    if(strcmp(script_type, "int"))
    {
        int i;
        char* accepted_types[] = {
            "int",
            "short",
            "long",
            "char"
        };
        for(i = 0; i != 4; ++i)
            if(strstr(type, accepted_types[i]))
                break;
        if(i == 4)
            return 0;
        /* 
         * Reaching this point means an integer type was recognized.
         * Reject any form of pointers */
        if(strstr(type, "*"))
            return 0;
        return 1;
    }
    
    /* floats */
    if(!strcmp(script_type, "float"))
    {
        int i;
        char* accepted_types[] = {
            "float",
            "double"
        };
        for(i = 0; i != 2; ++i)
            if(strstr(type, accepted_types[i]))
                break;
        if(i == 2)
            return 0;
        /* 
         * Reaching this point means a floatign point type was recognized.
         * Reject any form of pointers */
        if(strstr(type, "*"))
            return 0;
        return 1;
    }
    
    /* nothings */
    if(!strcmp(script_type, "none"))
    {
        if(!strstr(type, "void"))
            return 0;
        /* reject pointer types */
        if(strstr(type, "*"))
            return 0;
        return 1;
    }
    
    return 0;
}
