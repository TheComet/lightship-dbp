#include "util/pstdint.h"
#include "util/unordered_vector.h"
#include "framework/game.h"

extern uint32_t global_hash;

struct plugin_t;
struct game_t;

struct glob_parser_t
{
    struct unordered_vector_t open_docs;
    uint32_t doc_guid_counter;
};

struct glob_t
{
    struct plugin_t* plugin;
    struct glob_parser_t parser;
};

void
glob_create(struct game_t* game);

void
glob_destroy(struct game_t* game);

#define get_global(game) ((struct glob_t*)game_get_global(game, global_hash))
