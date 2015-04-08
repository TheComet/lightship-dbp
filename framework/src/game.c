#include "framework/game.h"
#include "framework/plugin_manager.h"
#include "framework/services.h"
#include "framework/events.h"
#include "util/memory.h"
#include "util/log.h"
#include "util/string.h"
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
    memset(game, 0, sizeof(struct game_t));
    
    /* initialise game object */
    game->name = malloc_string(name);
    list_init_list(&game->plugins);
    map_init_map(&game->services);
    map_init_map(&game->events);
    map_init_map(&game->global_data);
    
    /* initialise plugin manager, services, and events for this game instance */
    if(!framework_init(game))
        return 0;
    if(!services_init(game))
        return 0;
    if(!events_init(game))
        return 0;
    
    return game;
}

/* ------------------------------------------------------------------------- */
void
game_destroy(struct game_t* game)
{
    assert(game);
    assert(game->name);
    
    /* deinit plugin manager, services, and events (in reverse order) */
    events_deinit(game);
    services_deinit(game);
    framework_deinit(game);

    /* clean up data held by game object */
    map_clear_free(&game->global_data);
    map_clear_free(&game->events);
    map_clear_free(&game->services);
    list_clear(&game->plugins);
    free_string(game->name);
    
    FREE(game);
}

/* ------------------------------------------------------------------------- */
char
game_connect(struct game_t* game, const char* address)
{
    puts("stub: game_connect()");
    return 1;
}

/* ------------------------------------------------------------------------- */
void
game_disconnect(struct game_t* game)
{
}
