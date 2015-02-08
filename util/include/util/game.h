#include "util/config.h"
#include "util/map.h"

struct game_t
{
    char* name;
    struct map_t plugins;
    struct map_t services;
    struct map_t events;
};

LIGHTSHIP_PUBLIC_API struct game_t*
game_create(const char* name);

LIGHTSHIP_PUBLIC_API void
game_destroy(struct game_t* game);
