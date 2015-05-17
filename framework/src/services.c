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
        service->plugin = plugin;
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
        if(!(node = ptree_add_node(&plugin->game->services, directory, service)))
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
    struct game_t* game;

    assert(service);
    assert(service->plugin);
    assert(service->plugin->game);
    assert(service->directory);

    game = service->plugin->game;
    if(!(node = ptree_get_node(&game->services, service->directory)))
    {
        llog(LOG_ERROR, game, NULL, 5, "Attempted to destroy the "
            "service \"", service->directory, "\", but the associated game "
            "object with name \"", game->name, "\" doesn't own it! "
            "The service will not be destroyed.");
        return;
    }

    /* destroying the node will free the service using ptree's free function */
    ptree_destroy(node);
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
