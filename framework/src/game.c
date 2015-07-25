#include "framework/game.h"
#include "framework/plugin_manager.h"
#include "framework/plugin.h"
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

static struct bsthv_t g_games;

/* ------------------------------------------------------------------------- */
void
game_init(void)
{
	bsthv_init_bsthv(&g_games);
}

/* ------------------------------------------------------------------------- */
void
game_deinit(void)
{
	/* TODO free any left over games */
	bsthv_clear_free(&g_games);
}

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
		llog(LOG_INFO, NULL, NULL, "Creating game \"%s\" with mode \"%s\"", name, game_mode_str);
	}

	/* allocate game object */
	game = (struct game_t*)MALLOC(sizeof(struct game_t));
	if(!game)
		OUT_OF_MEMORY("game_create()", NULL);
	memset(game, 0, sizeof(struct game_t));

	for(;;)
	{
		game->network_role = net_role;

		/* initialise the game's global data container */
		bstv_init_bstv(&game->global_data);

		/* The initial state of the game is paused. The user must call
		 * game_start() to launch the game */
		game->state = GAME_STATE_PAUSED;

		/* copy game name */
		if(!(game->name = malloc_string(name)))
			break;

		/* if server, try to set up connection now */
		if(net_role == GAME_HOST)
		{
			game->connection = net_host_udp("3190", 20);
			if(!game->connection)
			{
				llog(LOG_ERROR, NULL, NULL, "Failed to host connection");
				break;
			}
		}

		/* initialise all services for this game */
		if(!llog_init(game))
			break;
		if(!plugin_manager_init(game))
			break;
		if(!service_init(game))
			break;
		if(!events_init(game))
			break;

		/* add to global list of games */
		if(!bsthv_insert(&g_games, name, game))
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

	/* remove game from global list */
	bsthv_erase(&g_games, game->name);

	/* disconnect the game */
	game_disconnect(game);

	/* deinit plugin manager, services, and events (in reverse order) */
	plugin_manager_deinit(game);
	events_deinit(game);
	service_deinit(game);

	/* clean up data held by game object */
	bstv_clear_free(&game->global_data);
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
	EVENT_FIRE0(game->event.start);
	game->state = GAME_STATE_RUNNING;
}

/* ------------------------------------------------------------------------- */
void
game_pause(struct game_t* game)
{
	EVENT_FIRE0(game->event.pause);
	game->state = GAME_STATE_PAUSED;
}

/* ------------------------------------------------------------------------- */
void
game_exit(struct game_t* game)
{
	EVENT_FIRE0(game->event.exit);
	game->state = GAME_STATE_TERMINATED;
}

/* ------------------------------------------------------------------------- */
void
games_run_all(void)
{
	while(g_games.vector.count)
	{
		/* update indiviual game loops */
		main_loop_do_loop();

		/* if the game wishes to terminate, destroy it */
		BSTHV_FOR_EACH(&g_games, struct game_t, key, game)
			if(game->state == GAME_STATE_TERMINATED)
			{
				game_destroy(game);
				goto break_g_games_for_each;
			}
		BSTHV_END_EACH
		break_g_games_for_each:

		/* HACK: If the localclient game is destroyed and the only game left
		 * is the localhost instance, it should be safe to assume we can
		 * quit */
		if(bsthv_count(&g_games) == 1)
		{
			BSTHV_FOR_EACH(&g_games, struct game_t, key, game)
				game_exit(game);
			BSTHV_END_EACH
		}
	}
}

/* ------------------------------------------------------------------------- */
void
game_dispatch_stats(uint32_t render_fps, uint32_t tick_fps)
{
	BSTHV_FOR_EACH(&g_games, struct game_t, key, game)
		EVENT_FIRE2(game->event.stats, render_fps, tick_fps);
	BSTHV_END_EACH
}

/* ------------------------------------------------------------------------- */
void
game_dispatch_render(void)
{
	BSTHV_FOR_EACH(&g_games, struct game_t, key, game)
		EVENT_FIRE0(game->event.render);
	BSTHV_END_EACH
}

/* ------------------------------------------------------------------------- */
void
game_dispatch_tick(void)
{
	BSTHV_FOR_EACH(&g_games, struct game_t, key, game)
		EVENT_FIRE0(game->event.tick);
	BSTHV_END_EACH
}

/* ------------------------------------------------------------------------- */
SERVICE(game_start_wrapper)
{
	game_start(service->plugin->game);
}

SERVICE(game_pause_wrapper)
{
	game_pause(service->plugin->game);
}

SERVICE(game_exit_wrapper)
{
	game_exit(service->plugin->game);
}
