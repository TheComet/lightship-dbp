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

/* ------------------------------------------------------------------------- */
char
service_init(struct game_t* game)
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
service_deinit(struct game_t* game)
{
    ptree_destroy_keep_root(&game->services);
}

/* ------------------------------------------------------------------------- */
struct service_t*
service_create(struct plugin_t* plugin,
               const char* directory,
               const service_func exec,
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
        service->type_info.has_unknown_types = 0; /* will be set to 1 during
                                                   * parsing if an unknown
                                                   * type is found */

        /* plugin object keeps track of all created services */
        if(!unordered_vector_push(&plugin->services, &service))
            break;

        /* copy directory */
        if(!(service->directory = malloc_string(directory)))
            break;

        /* copy return type info */
        if(!(service->type_info.ret_type_str = malloc_string(ret_type)))
            break;
        service->type_info.ret_type = service_get_type_from_string(ret_type);
        if(service->type_info.ret_type == SERVICE_TYPE_UNKNOWN)
            service->type_info.has_unknown_types = 1;

        /* create argument type vectors */
        if(!(service->type_info.argv_type_str = (char**)MALLOC(argc * sizeof(char*))))
            break;
        memset(service->type_info.argv_type_str, 0, argc * sizeof(char*));
        if(!(service->type_info.argv_type = (service_type_e*)MALLOC(argc * sizeof(service_type_e))))
            break;
        memset(service->type_info.argv_type, 0, argc * sizeof(service_type_e));

        /* copy argument type strings */
        {   int i;
            for(i = 0; i != argc; ++i)
            {
                if(!(service->type_info.argv_type_str[i] = malloc_string(argv[i])))
                    break;
                service->type_info.argv_type[i] = service_get_type_from_string(argv[i]);
                if(service->type_info.argv_type[i] == SERVICE_TYPE_UNKNOWN)
                    service->type_info.has_unknown_types = 1;

                ++service->type_info.argc;
            }
            if(i != argc)
                break;
        }

        /* create node in game's service directory - want to do this last
         * because ptree_remove_node uses malloc() */
        if(!(node = ptree_add_node(&service->game->services, directory, service)))
            break;

        /* NOTE: don't MALLOC() past this point ----------------------- */

        /* set the node's free function to service_free() to make deleting
         * nodes easier */
        ptree_set_free_func(node, (ptree_free_func)service_free);

        /* success! */
        return service;
    }

    /* something went wrong, clean up everything */

    /* remove from plugin's list of services */
    unordered_vector_erase_element(&plugin->services, &service);

    /* free type info argument string vector */
    if(service->type_info.argv_type_str)
    {   int i;
        for(i = 0; i != argc; ++i)
        {
            if(service->type_info.argv_type_str[i])
                free_string(service->type_info.argv_type_str[i]);
        }
        FREE(service->type_info.argv_type_str);
    }

    /* free type info argument vectors */
    if(service->type_info.argv_type)
        FREE(service->type_info.argv_type);

    /* free return type info */
    if(service->type_info.ret_type_str)
        free_string(service->type_info.ret_type_str);

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
    assert(service->type_info.ret_type_str);
    assert(service->type_info.argv_type_str);

    free_string(service->directory);
    free_string((char*)service->type_info.ret_type_str);
    for(i = 0; i != service->type_info.argc; ++i)
        free_string((char*)service->type_info.argv_type_str[i]);
    FREE(service->type_info.argv_type_str);
    FREE(service->type_info.argv_type);
    FREE(service);
}

/* ------------------------------------------------------------------------- */
uint32_t
service_destroy_all_matching(const char* pattern)
{
    return 0; /* TODO implement */
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
    if(service->type_info.argc != argv->count)
    {
        char argc_provided[sizeof(int)*8+1];
        char argc_required[sizeof(int)*8+1];
        sprintf(argc_provided, "%d", (int)argv->count);
        sprintf(argc_required, "%d", service->type_info.argc);
        llog(LOG_ERROR, service->game, NULL, 3, "Cannot create argument list for service \"",
             service->directory, "\": Wrong number of arguments");
        llog(LOG_ERROR, service->game, NULL, 2, "    Required: ", argc_required);
        llog(LOG_ERROR, service->game, NULL, 2, "    Provided: ", argc_provided);
        return NULL;
    }

    /* create void** argument vector */
    ret = (void**)MALLOC(service->type_info.argc * sizeof(void*));
    if(!ret)
        OUT_OF_MEMORY("service_create_argument_list_from_strings()", NULL);
    memset(ret, 0, service->type_info.argc * sizeof(void*));
    {
        int i = 0;
        char failed = 0;
        ORDERED_VECTOR_FOR_EACH(argv, const char*, str_p)
        {
            const char* str = *str_p;
            service_type_e type = service_get_type_from_string(service->type_info.argv_type_str[i]);
            switch(type)
            {
                /*
                 * NOTE: ret[i] *MUST* be assigned a value returned from
                 * MALLOC(), for it is later passed to FREE() upon
                 * destruction. We would use malloc_string() and
                 * strtowcs(), but that would require us to use free_string()
                 * instead of FREE().
                 */
                case SERVICE_TYPE_STRING:
                    ret[i] = MALLOC((strlen(str) + 1) * sizeof(char));
                    strcpy(ret[i], str);
                    break;
                case SERVICE_TYPE_WSTRING:
                    ret[i] = MALLOC((wcslen((const wchar_t*)str) + 1) * sizeof(wchar_t));
                    wcscpy(ret[i], (const wchar_t*)str);
                    ret[i] = strtowcs(str);
                    break;
/* ------------------------------------------------------------------------- */
                case SERVICE_TYPE_INT8:
                    ret[i] = MALLOC(sizeof(int8_t));
                    *((int8_t*)ret[i]) = (int8_t)atoi(str);
                    break;
                case SERVICE_TYPE_UINT8:
                    ret[i] = MALLOC(sizeof(uint8_t));
                    *((uint8_t*)ret[i]) = (uint8_t)atoi(str);
                    break;
                case SERVICE_TYPE_INT16:
                    ret[i] = MALLOC(sizeof(int16_t));
                    *((int16_t*)ret[i]) = (int16_t)atoi(str);
                    break;
                case SERVICE_TYPE_UINT16:
                    ret[i] = MALLOC(sizeof(uint16_t));
                    *((uint16_t*)ret[i]) = (uint16_t)atoi(str);
                    break;
                case SERVICE_TYPE_INT32:
                    ret[i] = MALLOC(sizeof(int32_t));
                    *((int32_t*)ret[i]) = (int32_t)atoi(str);
                    break;
                case SERVICE_TYPE_UINT32:
                    ret[i] = MALLOC(sizeof(uint32_t));
                    *((uint32_t*)ret[i]) = (uint32_t)atoi(str);
                    break;
                case SERVICE_TYPE_INT64:
                    ret[i] = MALLOC(sizeof(int64_t));
                    *((int64_t*)ret[i]) = (int64_t)atoi(str);
                    break;
                case SERVICE_TYPE_UINT64:
                    ret[i] = MALLOC(sizeof(uint64_t));
                    *((uint64_t*)ret[i]) = (uint64_t)atoi(str);
                    break;
/* ------------------------------------------------------------------------- */
                case SERVICE_TYPE_INTPTR:
                    ret[i] = MALLOC(sizeof(intptr_t));
                    *((intptr_t*)ret[i]) = (intptr_t)atoi(str);
                    break;
/* ------------------------------------------------------------------------- */
                case SERVICE_TYPE_FLOAT:
                    ret[i] = MALLOC(sizeof(float));
                    *((float*)ret[i]) = (float)atof(str);
                    break;
                case SERVICE_TYPE_DOUBLE:
                    ret[i] = MALLOC(sizeof(double));
                    *((double*)ret[i]) = atof(str);
                    break;
/* ------------------------------------------------------------------------- */
                case SERVICE_TYPE_NONE:
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
    for(i = 0; i != service->type_info.argc; ++i)
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
    if(argc != service->type_info.argc)
        return 0;
    /* verify return type */
    if(!ret_type || strcmp(ret_type, service->type_info.ret_type_str))
        return 0;
    /* verify argument types */
    for(i = 0; i != argc; ++i)
        if(!argv[i] || strcmp(argv[i], service->type_info.argv_type_str[i]))
            return 0;

    /* valid! */
    return 1;
}

/* ------------------------------------------------------------------------- */
/* Static functions */
/* ------------------------------------------------------------------------- */
service_type_e
service_get_type_from_string(const char* type)
{
    /* integers */
    if(strstr(type, "int"))
    {
        /*
         * Here we inspect the string to be any of the following:
         * int, int8_t, int16_t, int32_t, int64_t, intptr_t and all of their
         * unsigned counterparts.
         *
         */

        /* default to an int32 */
        service_type_e ret = SERVICE_TYPE_INT32;

        /* reject pointer types */
        if(strstr(type, "*"))
            return SERVICE_TYPE_UNKNOWN;
        /* intptr_t */
        if(strstr(type, "intptr"))
            ret = SERVICE_TYPE_INTPTR;
        /* number of bits */
        else if(strstr(type, "8"))
            ret = SERVICE_TYPE_INT8;
        else if(strstr(type, "16"))
            ret = SERVICE_TYPE_INT16;
        else if(strstr(type, "32"))
            ret = SERVICE_TYPE_INT32;
        else if(strstr(type, "64"))
            ret = SERVICE_TYPE_INT64;

        /* sign */
        if(strchr(type, 'u'))
            ++ret;
        return ret;
    }

    /* strings (or an int8/uint8) */
    {
        /* any form of wchar_t* is acceptable */
        if(strstr(type, "wchar_t"))
        {
            /* make sure it's not actually a wchar_t** */
            int num_chrs = 0;
            const char* str = type;
            /* one-liner for counting the number of '*' characters in a string
             * NOTE: modifies "str" */
            for(num_chrs = 0; str[num_chrs]; str[num_chrs] == '*' ? num_chrs++ : *str++);
            /* make sure it is a wchar_t* */
            if(num_chrs == 1)
                return SERVICE_TYPE_WSTRING;
            return SERVICE_TYPE_UNKNOWN;
        }

        /* any form of char* is acceptable */
        if(strstr(type, "char"))
        {
            /* make sure it's not actually a char** */
            int num_chrs = 0;
            const char* str = type;
            /* one-liner for counting the number of '*' characters in a string
             * NOTE: modifies "str" */
            for(num_chrs = 0; str[num_chrs]; str[num_chrs] == '*' ? num_chrs++ : *str++);
            if(num_chrs >= 2)
                return SERVICE_TYPE_UNKNOWN;
            /* make sure it is a char* */
            if(num_chrs == 1)
                return SERVICE_TYPE_STRING;

            /* it's only a char - determine if signed or unsigned, and return
             * either int8 or uint8 */
            if(strstr(type, "u"))
                return SERVICE_TYPE_UINT8;
            return SERVICE_TYPE_INT8;
        }
    }

    /* floats */
    if(strstr(type, "float"))
    {
        /* make sure it's not actually a float* */
        if(strstr(type, "*"))
            return SERVICE_TYPE_UNKNOWN;
        return SERVICE_TYPE_FLOAT;
    }

    /* doubles */
    if(strstr(type, "double"))
    {
        /* make sure it's not actually a float* */
        if(strstr(type, "*"))
            return SERVICE_TYPE_UNKNOWN;
        return SERVICE_TYPE_DOUBLE;
    }

    /* none/void */
    if(strstr(type, "void"))
    {
        /* reject pointer types */
        if(strstr(type, "*"))
            return SERVICE_TYPE_UNKNOWN;
        return SERVICE_TYPE_NONE;
    }

    /* unknown */
    return SERVICE_TYPE_UNKNOWN;
}
