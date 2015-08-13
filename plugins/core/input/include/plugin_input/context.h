#include "util/pstdint.h"
#include "util/unordered_vector.h"
#include "framework/game.h"

extern uint32_t context_hash;

struct plugin_t;
struct game_t;

struct context_parser_t
{
	struct unordered_vector_t open_docs;
	uint32_t doc_guid_counter;
};

struct context_t
{
	struct plugin_t* plugin;
	struct context_parser_t parser;
};

void
context_create(struct game_t* game);

void
context_destroy(struct game_t* game);

#define get_context(game) ((struct context_t*)game_get_from_context_store(game, context_hash))
