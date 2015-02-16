#include "util/pstdint.h"
#include "plugin_manager/game.h"
#include "plugin_main_loop/main_loop.h"

extern uint32_t global_hash;

struct plugin_t;
struct game_t;

struct glob_events_t
{
    struct event_t* render;
    struct event_t* update;
    struct event_t* stats;
    struct event_t* start;
    struct event_t* stop;
};

struct glob_plugin_t
{
    struct plugin_t* plugin;
};

struct glob_t
{
    struct plugin_t* plugin;
    struct glob_events_t events;
    struct main_loop_t main_loop;
};

void
glob_create(struct game_t* game);

void
glob_destroy(struct game_t* game);

#define get_global(game) ((struct glob_t*)game_get_global(game, global_hash))
