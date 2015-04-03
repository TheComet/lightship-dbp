#include "plugin_manager/config.h"
#include "util/map.h"
#include "util/linked_list.h"

typedef enum game_network_role_e
{
    GAME_CLIENT = 1,
    GAME_HOST = 2
} game_network_role_e;

struct glob_t;

struct game_t
{
    char* name;
    struct list_t plugins;      /* list of active plugins used by this game */
    struct map_t services;      /* maps service names to active services used by this game */
    struct map_t events;        /* maps event names to active events used by this game */
    struct map_t global_data;   /* maps hashed plugin names to glob structs used by this game */
};

PLUGIN_MANAGER_PUBLIC_API struct game_t*
game_create(const char* name, game_network_role_e net_role);

PLUGIN_MANAGER_PUBLIC_API void
game_destroy(struct game_t* game);

PLUGIN_MANAGER_PUBLIC_API void
game_connect(struct game_t* game, const char* address);

PLUGIN_MANAGER_PUBLIC_API void
game_disconnect(struct game_t* game);

#define game_add_global(game, hash, glob) map_insert(&(game)->global_data, hash, glob)
#define game_get_global(game, hash) map_find(&(game)->global_data, hash)
#define game_remove_global(game, hash) map_erase(&(game)->global_data, hash)
