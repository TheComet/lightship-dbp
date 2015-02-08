#include "util/game.h"
#include "util/memory.h"
#include "util/log.h"
#include "util/string.h"
#include <string.h>
#include <assert.h>

/* ------------------------------------------------------------------------- */
struct game_t*
game_create(const char* name)
{
    struct game_t* game;
    
    assert(name);

    game = (struct game_t*)MALLOC(sizeof(struct game_t));
    if(!game)
        OUT_OF_MEMORY("game_create()", NULL);
    memset(game, 0, sizeof(struct game_t));
    map_init_map(&game->plugins);
    map_init_map(&game->services);
    map_init_map(&game->events);
    game->name = malloc_string(name);
    
    return game;
}

/* ------------------------------------------------------------------------- */
void
game_destroy(struct game_t* game)
{
    assert(game);
    assert(game->name);

    map_clear_free(&game->events);
    map_clear_free(&game->services);
    map_clear_free(&game->plugins);
    free_string(game->name);
    FREE(game);
}
