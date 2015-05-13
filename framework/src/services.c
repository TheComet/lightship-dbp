#include "framework/game.h"
#include "framework/plugin.h"
#include "framework/services.h"
#include "util/hash.h"
#include "framework/log.h"
#include "util/map.h"
#include "util/memory.h"
#include "util/string.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <wchar.h>

static void
service_free(struct service_t* service);

static service_script_type_e
service_get_c_type_equivalent_from_script_type(const char* type);

static service_script_type_e
service_get_c_type_equivalent_from_service_type(const char* type);

/* ------------------------------------------------------------------------- */
char
services_register_core_services(struct game_t* game)
{
    assert(game);

    ptree_init_ptree(&game->services, NULL);

    /* ------------------------------------------------------------------------
     * Register built-in services
     * --------------------------------------------------------------------- */

    for(;;)
    {
        if(!(game->service.start = service_create(game->core, "start", game_start_wrapper, "void", 0, NULL))) break;
        if(!(game->service.pause = service_create(game->core, "pause", game_pause_wrapper, "void", 0, NULL))) break;
        if(!(game->service.exit  = service_create(game->core, "exit",  game_exit_wrapper,  "void", 0, NULL))) break;

        return 1;
    }

    return 0;
}

/* ------------------------------------------------------------------------- */
void
services_deinit(struct game_t* game)
{
    /* TODO Automatically unregister any left over services? */
    ptree_destroy_keep_root(&game->services);
}

/* ------------------------------------------------------------------------- */
struct service_t*
service_create(struct plugin_t* plugin,
               const char* directory,
               const service_callback_func exec,
               const char* ret_type,
               const int argc,
               const char** argv)
{
    struct service_t* service;
    struct ptree_t* node;

    assert(plugin);
    assert(plugin->game);
    assert(directory);
    assert(exec);
    assert(ret_type);
    if(argc)
        assert(argv);

    /* allocate and initialise service object */
    if(!(service = (struct service_t*)MALLOC(sizeof(struct service_t))))
        OUT_OF_MEMORY("service_create()", NULL);
    memset(service, 0, sizeof(struct service_t));

    /* if anything fails, break */
    for(;;)
    {
        service->game = plugin->game;
        service->exec = exec;

        /* copy directory */
        if(!(service->directory = malloc_string(directory)))
            break;

        /* plugin object keeps track of all created services */
        if(!unordered_vector_push(&plugin->services, &service))
            break;

        /* copy return type info */
        if(!(service->ret_type = malloc_string(ret_type)))
            break;

        /* create argument type vector */
        if(!(service->argv_type = (char**)MALLOC(argc * sizeof(char*))))
            break;
        memset(service->argv_type, 0, argc * sizeof(char*));

        /* copy argument types */
        {   int i;
            for(i = 0; i != argc; ++i)
            {
                if(!(service->argv_type[i] = malloc_string(argv[i])))
                    break;
                ++service->argc;
            }
            if(i != argc)
                break;
        }

        /* create node in game's service directory - want to do this last
         * because ptree_remove_node uses malloc() */
        if(!(node = ptree_add_node(&service->game->services, directory, service)))
            break;

        /* success! */
        return service;
    }

    /* something went wrong, clean up everything */
    if(service->argv_type)
    {   int i;
        for(i = 0; i != argc; ++i)
        {
            if(service->argv_type[i])
                free_string(service->argv_type[i]);
        }
        FREE(service->argv_type);
    }

    if(service->ret_type)
        free_string(service->ret_type);

    if(service->directory)
        free_string(service->directory);

    return NULL;
}

/* ------------------------------------------------------------------------- */
void
service_destroy(struct service_t* service)
{
    struct ptree_t* node;

    assert(service);
    assert(service->game);
    assert(service->directory);

    if(!(node = ptree_get_node(&service->game->services, service->directory)))
    {
        llog(LOG_ERROR, service->game, NULL, 5, "Attempted to destroy the "
            "service \"", service->directory, "\", but the associated game "
            "object with name \"", service->game->name, "\" doesn't own it! "
            "The service will not be destroyed.");
        return;
    }

    /* unlink service and destroy node */
    node->value = NULL;
    ptree_destroy(node);

    service_free(service);
}

/* ------------------------------------------------------------------------- */
static void
service_free(struct service_t* service)
{
    uint32_t i;

    assert(service);
    assert(service->directory);
    assert(service->ret_type);
    assert(service->argv_type);

    free_string(service->directory);
    free_string((char*)service->ret_type);
    for(i = 0; i != service->argc; ++i)
        free_string((char*)service->argv_type[i]);
    FREE(service->argv_type);
    FREE(service);
}

/* ------------------------------------------------------------------------- */
uint32_t
service_destroy_all_matching(const char* pattern)
{
    return 0;
}

/* ------------------------------------------------------------------------- */
struct service_t*
service_get(struct game_t* game, const char* directory)
{
    struct ptree_t* node;

    assert(game);
    assert(directory);

    if(!(node = ptree_get_node(&game->services, directory)))
        return NULL;
    assert(node->value);
    return (struct service_t*)node->value;
}

/* ------------------------------------------------------------------------- */
void**
service_create_argument_list_from_strings(struct service_t* service, struct ordered_vector_t* argv)
{
    void** ret;

    assert(service);
    assert(service->directory);
    assert(argv);

    /* check argument count */
    if(service->argc != argv->count)
    {
        char argc_provided[sizeof(int)*8+1];
        char argc_required[sizeof(int)*8+1];
        sprintf(argc_provided, "%d", (int)argv->count);
        sprintf(argc_required, "%d", service->argc);
        llog(LOG_ERROR, service->game, NULL, 3, "Cannot create argument list for service \"",
             service->directory, "\": Wrong number of arguments");
        llog(LOG_ERROR, service->game, NULL, 2, "    Required: ", argc_required);
        llog(LOG_ERROR, service->game, NULL, 2, "    Provided: ", argc_provided);
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
                /*
                 * NOTE: ret[i] *MUST* be assigned a value returned from
                 * MALLOC(), for it is later passed to FREE() upon
                 * destruction. We would use malloc_string() and
                 * strtowcs(), but that would require us to use free_string()
                 * instead of FREE().
                 */
                case SERVICE_SCRIPT_TYPE_STRING:
                    ret[i] = MALLOC((strlen(str) + 1) * sizeof(char));
                    strcpy(ret[i], str);
                    break;
                case SERVICE_SCRIPT_TYPE_WSTRING:
                    ret[i] = MALLOC((wcslen((const wchar_t*)str) + 1) * sizeof(wchar_t));
                    wcscpy(ret[i], (const wchar_t*)str);
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
                    llog(LOG_ERROR, service->game, NULL, 4, "Cannot create argument list for service \"",
                         service->directory, "\": Unknown type \"", str, "\"");
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
            /* one-liner for counting the number of '*' characters in a string
             * NOTE: modifies "str" */
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
            /* one-liner for counting the number of '*' characters in a string
             * NOTE: modifies "str" */
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
