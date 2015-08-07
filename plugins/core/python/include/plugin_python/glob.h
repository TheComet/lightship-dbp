/*!
 * @file glob.h
 * @brief Global data struct.
 *
 * TODO add description why this was necessary to do.
 */

#include "util/pstdint.h"
#include "framework/game.h"

extern uint32_t global_hash;

struct plugin_t;
struct game_t;

struct glob_t
{
	struct game_t* game;                /* handle of the game object that owns this plugin */
	struct plugin_t* plugin;            /* handle of the plugin object that owns this glob */
	/*struct glob_events_t events;
	struct glob_services_t services;*/
};

void
glob_create(struct game_t* game);

void
glob_destroy(struct game_t* game);

#define get_global(game) ((struct glob_t*)game_get_global(game, global_hash))
