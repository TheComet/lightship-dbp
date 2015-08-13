#include "util/pstdint.h"
#include "util/unordered_vector.h"
#include "framework/game.h"

extern uint32_t context_hash;

struct plugin_t;
struct game_t;

struct context_t
{
	struct plugin_t* plugin;
	struct game_t* game;
};

void
context_create(struct game_t* game);

void
context_destroy(struct game_t* game);

#define get_context(game) ((struct context_t*)game_get_from_context_store(game, context_hash))
