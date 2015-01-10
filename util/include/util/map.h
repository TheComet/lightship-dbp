#ifndef LIGHTSHIP_UTIL_MAP_H
#define LIGHTSHIP_UTIL_MAP_H

#include "util/pstdint.h"
#include "util/config.h"
#include "util/ordered_vector.h"

struct map_key_value_t
{
    uint32_t hash;
    void* value;
};

struct map_t
{
    struct ordered_vector_t vector;
};

LIGHTSHIP_PUBLIC_API struct map_t*
map_create(void);

LIGHTSHIP_PUBLIC_API void
map_destroy(struct map_t* map);

LIGHTSHIP_PUBLIC_API void
map_insert(struct map_t* map, uint32_t hash, void* value);

LIGHTSHIP_PUBLIC_API void*
map_find(struct map_t* map, uint32_t hash);

#endif /* LIGHTSHIP_UTIL_MAP_H */
