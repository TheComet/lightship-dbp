#include "framework/game.h"
#include "framework/plugin_manager.h"
#include "framework/services.h"
#include "framework/events.h"
#include "framework/log.h"
#include "framework/main_loop.h"
#include "util/memory.h"
#include "util/string.h"
#include "util/net.h"
#include "util/hash.h"
#include "thread_pool/thread_pool.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

static struct map_t* g_games = NULL;

/* ------------------------------------------------------------------------- */
struct game_t*
game_create(const char* name, game_network_role_e net_role)
{
    struct game_t* game;
    
    assert(name);
    
    /* inform log about game name and game mode */
    {
        char* game_mode_str;
        if(net_role == GAME_HOST)
            game_mode_str = "server";
        else
            game_mode_str = "client";
        llog(LOG_INFO, NULL, NULL, 5, "Creating game \"", name, "\" with mode \"", game_mode_str, "\"");
    }

    /* allocate game object */
    game = (struct game_t*)MALLOC(sizeof(struct game_t));
    if(!game)
        OUT_OF_MEMORY("game_create()", NULL);
    
    /* initialise game object */
    memset(game, 0, sizeof(struct game_t));
    game->name = malloc_string(name);
    game->network_role = net_role;
    
    /* initialise the game's global data container */
    map_init_map(&game->global_data);
    
    /* The initial state of the game is paused. The user must call game_start() 
     * to launch the game */
    game->state = GAME_STATE_PAUSED;
    
    for(;;)
    {
    
        /* if server, try to set up connection now */
        if(net_role == GAME_HOST)
        {
            game->connection = net_host_udp("3190", 20);
            if(!game->connection)
                break;
        }
        
        /* initialise all services for this game */
        if(!llog_init(game))
            break;
        if(!services_init(game))
            break;
        if(!events_init(game))
            break;
        if(!plugin_manager_init(game))
            break;
        
        /* add to global list of games */
        {
            uint32_t hash = hash_jenkins_oaat(name, strlen(name));
            if(!map_insert(g_games, hash, game))
                break;
        }
        
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
    uint32_t hash;
    
    assert(game);
    assert(game->name);
    
    /* remove game from global list */
    hash = hash_jenkins_oaat(game->name, strlen(game->name));
    map_erase(g_games, hash);
    
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

/* ------------------------------------------------------------------------- */
void
game_start(struct game_t* game)
{
    EVENT_FIRE_FROM_TEMP0(evt_start, game->event.start);
    game->state = GAME_STATE_RUNNING;
}

/* ------------------------------------------------------------------------- */
void
game_pause(struct game_t* game)
{
    EVENT_FIRE_FROM_TEMP0(evt_pause, game->event.pause);
    game->state = GAME_STATE_PAUSED;
}

/* ------------------------------------------------------------------------- */
void
game_exit(struct game_t* game)
{
    EVENT_FIRE_FROM_TEMP0(evt_exit, game->event.exit);
    game->state = GAME_STATE_TERMINATED;
}

/* ------------------------------------------------------------------------- */
void
games_run_all(void)
{
    while(g_games && g_games->vector.count)
    {
        /* update indiviual game loops */
        main_loop_do_loop();
        
        /* if the game wishes to terminate, destroy it */
        /* TODO
        { UNORDERED_VECTOR_FOR_EACH(g_games, struct game_t*, p_game)
        {
            if((*p_game)->state == GAME_STATE_TERMINATED)
            {
                game_destroy(*p_game);
                break;
            }
        }}*/
    }
}

/* ------------------------------------------------------------------------- */
void
game_dispatch_stats(uint32_t render_fps, uint32_t tick_fps)
{
    MAP_FOR_EACH(g_games, struct game_t, key, game)
    {
        EVENT_FIRE_FROM_TEMP2(evt_loop_stats, game->event.stats,
                              render_fps, tick_fps);
    }
}

/* ------------------------------------------------------------------------- */
void
game_dispatch_render(void)
{
    MAP_FOR_EACH(g_games, struct game_t, key, game)
    {
        EVENT_FIRE_FROM_TEMP0(evt_render, game->event.render);
    }
}

/* ------------------------------------------------------------------------- */
void
game_dispatch_tick(void)
{
    MAP_FOR_EACH(g_games, struct game_t, key, game)
    {
        EVENT_FIRE_FROM_TEMP0(evt_tick, game->event.tick);
    }
}

/* ------------------------------------------------------------------------- */
SERVICE(game_start_wrapper)
{
    game_start(service->game);
}

SERVICE(game_pause_wrapper)
{
    game_pause(service->game);
}

SERVICE(game_exit_wrapper)
{
    game_exit(service->game);
}
