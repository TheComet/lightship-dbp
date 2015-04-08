#include "framework/game.h"
#include "framework/plugin_manager.h"
#include "framework/services.h"
#include "framework/events.h"
#include "util/memory.h"
#include "util/log.h"
#include "util/string.h"
#include "util/net.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

/* ------------------------------------------------------------------------- */
struct game_t*
game_create(const char* name, game_network_role_e net_role)
{
    struct game_t* game;
    char* game_mode_str;
    
    assert(name);
    
    /* inform log about game name and game mode */
    if(net_role == GAME_HOST)
        game_mode_str = "server";
    else
        game_mode_str = "client";
    llog(LOG_INFO, NULL, 5, "Creating game \"", name, "\" with mode \"", game_mode_str, "\"");

    /* allocate game object */
    game = (struct game_t*)MALLOC(sizeof(struct game_t));
    if(!game)
        OUT_OF_MEMORY("game_create()", NULL);
    
    /* initialise game object */
    memset(game, 0, sizeof(struct game_t));
    game->name = malloc_string(name);
    game->network_role = net_role;
    map_init_map(&game->global_data);
    
    for(;;)
    {
    
        /* if server, try to set up connection now */
        if(net_role == GAME_HOST)
        {
            game->connection = net_host_udp("3190", 20);
            if(!game->connection)
                break;
        }
        
        /* initialise plugin manager, services, and events for this game instance */
        if(!services_init(game))
            break;
        if(!events_init(game))
            break;
        if(!plugin_manager_init(game))
            break;
        
        /* success! */
        return game;
    }
    
    game_destroy(game);
    
    return NULL;
}

/* ------------------------------------------------------------------------- */
void
game_destroy(struct game_t* game)
{
    assert(game);
    assert(game->name);
    
    /* disconnect the game */
    game_disconnect(game);
    
    /* deinit plugin manager, services, and events (in reverse order) */
    plugin_manager_deinit(game);
    events_deinit(game);
    services_deinit(game);

    /* clean up data held by game object */
    map_clear_free(&game->global_data);
    free_string(game->name);
    
    FREE(game);
}

/* ------------------------------------------------------------------------- */
char
game_connect(struct game_t* game, const char* address)
{
    game->connection = net_join_udp(address, "3190");
    if(!game->connection)
        return 0;
    return 1;
}

/* ------------------------------------------------------------------------- */
void
game_disconnect(struct game_t* game)
{
    if(game->connection)
        net_disconnect(game->connection);
    game->connection = NULL;
}
