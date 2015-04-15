#include "framework/game.h"
#include "framework/plugin_manager.h"
#include "framework/services.h"
#include "framework/events.h"
#include "framework/log.h"
#include "framework/main_loop.h"
#include "util/memory.h"
#include "util/string.h"
#include "util/net.h"
#include "thread_pool/thread_pool.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

static struct unordered_vector_t* g_games = NULL;

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
    llog(LOG_INFO, NULL, NULL, 5, "Creating game \"", name, "\" with mode \"", game_mode_str, "\"");

    /* allocate game object */
    game = (struct game_t*)MALLOC(sizeof(struct game_t));
    if(!game)
        OUT_OF_MEMORY("game_create()", NULL);
    
    /* initialise game object */
    memset(game, 0, sizeof(struct game_t));
    game->state = GAME_STATE_PAUSED;
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
        
        /* initialise all services for this game */
        if(!llog_init(game))
            break;
        if(!services_init(game))
            break;
        if(!events_init(game))
            break;
        if(!plugin_manager_init(game))
            break;
        
        /* add to global list of games - container may not be initialised yet */
        /* NOTE: Important: Only the pointers are stored in the vector */
        if(!g_games)
            g_games = unordered_vector_create(sizeof(struct game_t*));
        unordered_vector_push(g_games, &game);
        
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
    
    /* remove game from global list */
    { UNORDERED_VECTOR_FOR_EACH(g_games, struct game_t*, p_game)
    {
        if(*p_game == game)
        {
            unordered_vector_erase_element(g_games, p_game);
            break;
        }
    }}
    
    /* if the last element was removed from global list, clear free vector */
    if(g_games->count == 0)
        unordered_vector_destroy(g_games);
    
    /* disconnect the game */
    game_disconnect(game);
    
    /* deinit plugin manager, services, and events (in reverse order) */
    plugin_manager_deinit(game);
    events_deinit(game);
    services_deinit(game);
    llog_deinit(game);

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
game_run(struct game_t* game)
{
    game->state = GAME_STATE_RUNNING;
}

/* ------------------------------------------------------------------------- */
void
game_pause(struct game_t* game)
{
    game->state = GAME_STATE_PAUSED;
}

/* ------------------------------------------------------------------------- */
void
game_stop(struct game_t* game)
{
    game->state = GAME_STATE_TERMINATED;
}

/* ------------------------------------------------------------------------- */
void
games_run_all(void)
{
    struct thread_pool_t* pool = thread_pool_create(0, 0);
    
    while(g_games->count)
    {
        thread_pool_queue(pool, (thread_pool_job_func)main_loop_do_loop, NULL);
        thread_pool_wait_for_jobs(pool);
    }
    
    thread_pool_destroy(pool);
}

/* ------------------------------------------------------------------------- */
void
game_dispatch_stats(uint32_t render_fps, uint32_t tick_fps)
{
    UNORDERED_VECTOR_FOR_EACH(g_games, struct game_t*, p_game)
    {
        EVENT_FIRE_FROM_TEMP2(evt_loop_stats, (*p_game)->event.loop_stats,
                              render_fps, tick_fps);
    }
}

/* ------------------------------------------------------------------------- */
void
game_dispatch_render(void)
{
    UNORDERED_VECTOR_FOR_EACH(g_games, struct game_t*, p_game)
    {
        EVENT_FIRE_FROM_TEMP0(evt_render, (*p_game)->event.render);
    }
}

/* ------------------------------------------------------------------------- */
void
game_dispatch_tick(void)
{
    UNORDERED_VECTOR_FOR_EACH(g_games, struct game_t*, p_game)
    {
        EVENT_FIRE_FROM_TEMP0(evt_tick, (*p_game)->event.tick);
    }
}

