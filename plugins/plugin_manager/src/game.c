#include "plugin_manager/game.h"
#include "util/memory.h"
#include "util/log.h"
#include "util/string.h"
#include <string.h>
#include <assert.h>

/* ------------------------------------------------------------------------- */
struct game_t*
game_create(const char* name, game_network_role_e net_role)
{
    struct game_t* game;
    
    assert(name);

    game = (struct game_t*)MALLOC(sizeof(struct game_t));
    if(!game)
        OUT_OF_MEMORY("game_create()", NULL);
    memset(game, 0, sizeof(struct game_t));
    game->name = malloc_string(name);
    list_init_list(&game->plugins);
    map_init_map(&game->services);
    map_init_map(&game->events);
    map_init_map(&game->global_data);
    
    return game;
}

/* ------------------------------------------------------------------------- */
void
game_destroy(struct game_t* game)
{
    assert(game);
    assert(game->name);

    map_clear_free(&game->global_data);
    map_clear_free(&game->events);
    map_clear_free(&game->services);
    list_clear(&game->plugins);
    free_string(game->name);
    FREE(game);
}
