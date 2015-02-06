#include "util/services.h"
#include "util/memory.h"
#include "util/plugin.h"
#include "util/string.h"
#include "util/map.h"
#include "util/hash.h"
#include <util/log.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct map_t g_services;
char  g_service_internal_no_arg_dummy     = 0;

/*!
 * @brief Allocates and registers a new service. This is for internal use.
 * @param[in] full_name The full name, including namespace, of the service to
 * create and register.
 * @note *full_name* is owned by the service object after calling this function
 * and will be freed automatically. Therefore, you should pass a malloc'd string
 * as a parameter.
 * @param exec The function address of the callback function of the service.
 */
static char
service_malloc_and_register(char* full_name,
                            const service_callback_func exec,
                            const char* ret_type,
                            const int argc,
                            const char** argv);

static service_script_type_e
service_get_c_type_equivalent_from_script_type(const char* type);

static service_script_type_e
service_get_c_type_equivalent_from_service_type(const char* type);

/* ------------------------------------------------------------------------- */
void
services_init(void)
{
    map_init_map(&g_services);
    
    /* ------------------------------------------------------------------------
     * Register built-in services 
     * --------------------------------------------------------------------- */
    
    
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
                 const service_callback_func exec,
                 const char* ret_type,
                 const int argc,
                 const char** argv)
{
    char* full_name;
    
    assert(plugin);
    assert(name);
    assert(exec);
    assert(ret_type);

    /* check if service is already registered */
    full_name = cat_strings(3, plugin->info.name, ".", name);
    if(!full_name)
        return 0;
    if(service_get(full_name))
    {
        free_string(full_name);
        return 0;
    }

    if(!service_malloc_and_register(full_name, exec, ret_type, argc, argv))
        return 0;

    return 1;
}

/* ------------------------------------------------------------------------- */
static char
service_malloc_and_register(char* full_name,
                            const service_callback_func exec,
                            const char* ret_type,
                            const int argc,
                            const char** argv)
{
    struct service_t* service;
    
    assert(full_name);
    assert(exec);
    assert(ret_type);
    
    /* create service and add to list */
    service = (struct service_t*)MALLOC(sizeof(struct service_t));
    if(!service)
        OUT_OF_MEMORY("service_malloc_and_register()", 0);
    memset(service, 0, sizeof(struct service_t));
    service->name = full_name;
    service->exec = exec;
    
    /* copy type info */
    {
        int i;
        char** argv_tmp;
        
        /* copy return type */
        char* ret_type_tmp = malloc_string(ret_type);
        if(!ret_type_tmp)
        {
            service_free(service);
            return 0;
        }
        memcpy(&service->ret_type, &ret_type_tmp, sizeof(char*));
        
        /* create argument type vector */
        argv_tmp= (char**)MALLOC(argc * sizeof(char**));
        if(!argv_tmp)
        {
            service_free(service);
            OUT_OF_MEMORY("service_malloc_and_register()", 0);
        }
        memcpy(&service->argv_type, &argv_tmp, sizeof(char**));
    
        /* copy argument type vector */
        for(i = 0; i != argc; ++i)
        {
            argv_tmp[i] = malloc_string(argv[i]);
            if(!argv_tmp[i])
            {
                service_free(service);
                return 0;
            }
            ++service->argc;
        }
    }
    if(!map_insert(&g_services, hash_jenkins_oaat(full_name, strlen(full_name)), service))
        return 0;
    
    return 1;
}

/* ------------------------------------------------------------------------- */
void
service_free(struct service_t* service)
{
    uint32_t i;

    assert(service);
    assert(service->name);
    assert(service->ret_type);
    assert(service->argv_type);

    free_string(service->name);
    free_string((char*)service->ret_type);
    for(i = 0; i != service->argc; ++i)
        free_string((char*)service->argv_type[i]);
    FREE(service->argv_type);
    FREE(service);
}

/* ------------------------------------------------------------------------- */
char
service_unregister(const struct plugin_t* plugin,
                   const char* name)
{
    char* full_name;
    uint32_t hash;
    struct service_t* service;
    
    assert(plugin);
    assert(plugin->info.name);
    assert(name);

    full_name = cat_strings(3, plugin->info.name, ".", name);
    if(!full_name)
        return 0;

    /* remove service from map */
    hash = hash_jenkins_oaat(full_name, strlen(full_name));
    free_string(full_name);
    if(!(service = map_erase(&g_services, hash)))
        return 0;
    service_free(service);

    return 1;
}

/* ------------------------------------------------------------------------- */
char
service_unregister_all(const struct plugin_t* plugin)
{
    char* name;
    int len;
    
    assert(plugin);
    assert(plugin->info.name);

    name = cat_strings(2, plugin->info.name, ".");
    if(!name)
        return 0;

    len = strlen(plugin->info.name);
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
    
    return 1;
}

/* ------------------------------------------------------------------------- */
struct service_t*
service_get(const char* name)
{
    struct service_t* service;
    
    assert(name);

    if(!(service = map_find(&g_services, hash_jenkins_oaat(name, strlen(name)))))
        return 0;

    return service;
}

/* ------------------------------------------------------------------------- */
void**
service_create_argument_list_from_strings(struct service_t* service, struct ordered_vector_t* argv)
{
    void** ret;
    
    assert(service);
    assert(service->name);
    assert(argv);

    /* check argument count */
    if(service->argc != argv->count)
    {
        char argc_provided[sizeof(int)*8+1];
        char argc_required[sizeof(int)*8+1];
        sprintf(argc_provided, "%d", (int)argv->count);
        sprintf(argc_required, "%d", service->argc);
        llog(LOG_ERROR, NULL, 3, "Cannot create argument list for service \"", service->name,
                           "\": Wrong number of arguments");
        llog(LOG_ERROR, NULL, 2, "    Required: ", argc_required);
        llog(LOG_ERROR, NULL, 2, "    Provided: ", argc_provided);
        return NULL;
    }

    /* create void** argument vector */
    ret = (void**)MALLOC(service->argc * sizeof(void*));
    if(!ret)
        OUT_OF_MEMORY("service_create_argument_list_from_strings()", NULL);
    memset(ret, 0, service->argc * sizeof(void*));
    {
        int i = 0;
        char failed = 0;
        ORDERED_VECTOR_FOR_EACH(argv, const char*, str_p)
        {
            const char* str = *str_p;
            service_script_type_e type = service_get_c_type_equivalent_from_service_type(service->argv_type[i]);
            switch(type)
            {
                case SERVICE_SCRIPT_TYPE_STRING:
                    ret[i] = malloc_string(str);
                    break;
                case SERVICE_SCRIPT_TYPE_WSTRING:
                    ret[i] = strtowcs(str);
                    break;
/* ------------------------------------------------------------------------- */
                case SERVICE_SCRIPT_TYPE_INT8:
                    ret[i] = MALLOC(sizeof(int8_t));
                    *((int8_t*)ret[i]) = (int8_t)atoi(str);
                    break;
                case SERVICE_SCRIPT_TYPE_UINT8:
                    ret[i] = MALLOC(sizeof(uint8_t));
                    *((uint8_t*)ret[i]) = (uint8_t)atoi(str);
                    break;
                case SERVICE_SCRIPT_TYPE_INT16:
                    ret[i] = MALLOC(sizeof(int16_t));
                    *((int16_t*)ret[i]) = (int16_t)atoi(str);
                    break;
                case SERVICE_SCRIPT_TYPE_UINT16:
                    ret[i] = MALLOC(sizeof(uint16_t));
                    *((uint16_t*)ret[i]) = (uint16_t)atoi(str);
                    break;
                case SERVICE_SCRIPT_TYPE_INT32:
                    ret[i] = MALLOC(sizeof(int32_t));
                    *((int32_t*)ret[i]) = (int32_t)atoi(str);
                    break;
                case SERVICE_SCRIPT_TYPE_UINT32:
                    ret[i] = MALLOC(sizeof(uint32_t));
                    *((uint32_t*)ret[i]) = (uint32_t)atoi(str);
                    break;
                case SERVICE_SCRIPT_TYPE_INT64:
                    ret[i] = MALLOC(sizeof(int64_t));
                    *((int64_t*)ret[i]) = (int64_t)atoi(str);
                    break;
                case SERVICE_SCRIPT_TYPE_UINT64:
                    ret[i] = MALLOC(sizeof(uint64_t));
                    *((uint64_t*)ret[i]) = (uint64_t)atoi(str);
                    break;
/* ------------------------------------------------------------------------- */
                case SERVICE_SCRIPT_TYPE_INTPTR:
                    ret[i] = MALLOC(sizeof(intptr_t));
                    *((intptr_t*)ret[i]) = (intptr_t)atoi(str);
                    break;
/* ------------------------------------------------------------------------- */
                case SERVICE_SCRIPT_TYPE_FLOAT:
                    ret[i] = MALLOC(sizeof(float));
                    *((float*)ret[i]) = (float)atof(str);
                    break;
                case SERVICE_SCRIPT_TYPE_DOUBLE:
                    ret[i] = MALLOC(sizeof(double));
                    *((double*)ret[i]) = atof(str);
                    break;
/* ------------------------------------------------------------------------- */
                case SERVICE_SCRIPT_TYPE_NONE:
                    ret[i] = MALLOC(sizeof(char));
                    *((char*)ret[i]) = '\0';
                    break;
                default:
                    llog(LOG_ERROR, NULL, 4, "Cannot create argument list for service \"", service->name,
                                       "\": Unknown type \"", str, "\"");
                    failed = 1;
                    break;
            }
            
            ++i;
        }
        
        /* if any of the conversions failed, free the list and return NULL */
        if(failed)
        {
            service_destroy_argument_list(service, ret);
            ret = NULL;
        }
    }

    return ret;
}

/* ------------------------------------------------------------------------- */
void
service_destroy_argument_list(struct service_t* service, void** argv)
{
    uint32_t i;
    for(i = 0; i != service->argc; ++i)
        if(argv[i])
            FREE(argv[i]);
    FREE(argv);
}

/* ------------------------------------------------------------------------- */
char
service_do_typecheck(const struct service_t* service, const char* ret_type, uint32_t argc, const char** argv)
{
    uint32_t i;

    /* verify argument count */
    if(argc != service->argc)
        return 0;
    /* verify return type */
    if(!ret_type || strcmp(ret_type, service->ret_type))
        return 0;
    /* verify argument types */
    for(i = 0; i != argc; ++i)
        if(!argv[i] || strcmp(argv[i], service->argv_type[i]))
            return 0;
    
    /* valid! */
    return 1;
}

/* ------------------------------------------------------------------------- */
/* Static functions */
/* ------------------------------------------------------------------------- */
static service_script_type_e
service_get_c_type_equivalent_from_script_type(const char* type)
{
    if(strcmp(type, "string") == 0)
        return SERVICE_SCRIPT_TYPE_STRING;
    if(strcmp(type, "wstring") == 0)
        return SERVICE_SCRIPT_TYPE_WSTRING;
    if(strcmp(type, "int") == 0)
        return SERVICE_SCRIPT_TYPE_INT32;
    if(strcmp(type, "uint") == 0)
        return SERVICE_SCRIPT_TYPE_UINT32;
    if(strcmp(type, "float") == 0)
        return SERVICE_SCRIPT_TYPE_FLOAT;
    if(strcmp(type, "double") == 0)
        return SERVICE_SCRIPT_TYPE_DOUBLE;
    if(strcmp(type, "none") == 0)
        return SERVICE_SCRIPT_TYPE_NONE;

    return SERVICE_SCRIPT_TYPE_UNKNOWN;
}

/* ------------------------------------------------------------------------- */
static service_script_type_e
service_get_c_type_equivalent_from_service_type(const char* type)
{
    /* strings */
    {
        /* any form of char* is acceptable */
        if(strstr(type, "char*"))
        {
            /* make sure it's not actually a char** */
            int num_chrs = 0;
            const char* str = type;
            for(num_chrs = 0; str[num_chrs]; str[num_chrs] == '*' ? num_chrs++ : *str++);
            if(num_chrs >= 2)
                return SERVICE_SCRIPT_TYPE_UNKNOWN;
            return SERVICE_SCRIPT_TYPE_STRING;
        }
        
        /* any form of wchar_t* is acceptable */
        if(strstr(type, "wchar_t*"))
        {
            /* make sure it's not actually a wchar_t** */
            int num_chrs = 0;
            const char* str = type;
            for(num_chrs = 0; str[num_chrs]; str[num_chrs] == '*' ? num_chrs++ : *str++);
            if(num_chrs >= 2)
                return SERVICE_SCRIPT_TYPE_UNKNOWN;
            return SERVICE_SCRIPT_TYPE_WSTRING;
        }
    }
    
    /* integers */
    if(strstr(type, "int"))
    {
        service_script_type_e ret = SERVICE_SCRIPT_TYPE_UNKNOWN;

        /* reject pointer types */
        if(strstr(type, "*"))
            return SERVICE_SCRIPT_TYPE_UNKNOWN;
        /* intptr_t */
        if(strstr(type, "intptr"))
            return SERVICE_SCRIPT_TYPE_INTPTR;
        /* number of bits */
        if(strstr(type, "8"))
            ret = SERVICE_SCRIPT_TYPE_INT8;
        else if(strstr(type, "16"))
            ret = SERVICE_SCRIPT_TYPE_INT16;
        else if(strstr(type, "32"))
            ret = SERVICE_SCRIPT_TYPE_INT32;
        else if(strstr(type, "64"))
            ret = SERVICE_SCRIPT_TYPE_INT64;
        else
            return SERVICE_SCRIPT_TYPE_UNKNOWN;
        /* sign */
        if(strchr(type, 'u'))
            ++ret;
        return ret;
        
    }
    
    /* floats */
    if(strstr(type, "float"))
    {
        /* make sure it's not actually a float* */
        if(strstr(type, "*"))
            return SERVICE_SCRIPT_TYPE_UNKNOWN;
        return SERVICE_SCRIPT_TYPE_FLOAT;
    }
    
    /* doubles */
    if(strstr(type, "double"))
    {
        /* make sure it's not actually a float* */
        if(strstr(type, "*"))
            return SERVICE_SCRIPT_TYPE_UNKNOWN;
        return SERVICE_SCRIPT_TYPE_DOUBLE;
    }
    
    /* none/void */
    if(strstr(type, "void"))
    {
        /* reject pointer types */
        if(strstr(type, "*"))
            return SERVICE_SCRIPT_TYPE_UNKNOWN;
        return SERVICE_SCRIPT_TYPE_NONE;
    }
    
    /* unknown */
    return SERVICE_SCRIPT_TYPE_UNKNOWN;
}
