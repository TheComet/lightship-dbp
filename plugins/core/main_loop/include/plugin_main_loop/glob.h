#include "util/pstdint.h"
#include "util/unordered_vector.h"
#include "plugin_manager/game.h"
#include "plugin_main_loop/main_loop.h"

extern uint32_t global_hash;

struct plugin_t;
struct game_t;

struct glob_plugin_t
{
    struct plugin_t* plugin;
};

struct glob_t
{
    struct glob_plugin_t plugin;
    struct main_loop_t main_loop;
};

void
glob_create(struct game_t* game);

void
glob_destroy(struct game_t* game);

#define get_global(game) ((struct glob_t*)game_get_global(game, global_hash))
