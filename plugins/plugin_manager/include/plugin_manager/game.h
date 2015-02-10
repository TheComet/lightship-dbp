#include "plugin_manager/config.h"
#include "util/map.h"
#include "util/linked_list.h"

struct glob_t;

struct game_t
{
    char* name;
    struct list_t plugins;
    struct map_t services;
    struct map_t events;
    struct map_t global_data;
};

PLUGIN_MANAGER_PUBLIC_API struct game_t*
game_create(const char* name);

PLUGIN_MANAGER_PUBLIC_API void
game_destroy(struct game_t* game);

#define game_add_global(game, hash, glob) map_insert(&(game)->global_data, hash, glob)
#define game_get_global(game, hash) map_find(&(game)->global_data, hash)
#define game_remove_global(game, hash) map_erase(&(game)->global_data, hash)
