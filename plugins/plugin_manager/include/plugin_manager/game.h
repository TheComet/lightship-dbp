#include "plugin_manager/config.h"
#include "util/map.h"
#include "util/linked_list.h"

struct game_t
{
    char* name;
    struct list_t plugins;
    struct map_t services;
    struct map_t events;
};

PLUGIN_MANAGER_PUBLIC_API struct game_t*
game_create(const char* name);

PLUGIN_MANAGER_PUBLIC_API void
game_destroy(struct game_t* game);
