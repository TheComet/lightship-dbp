#ifndef LIGHTSHIP_UTIL_MAP_H
#define LIGHTSHIP_UTIL_MAP_H

#include "util/pstdint.h"
#include "util/config.h"
#include "util/ordered_vector.h"

C_HEADER_BEGIN

struct map_key_value_t
{
    intptr_t hash;
    void* value;
};

struct map_t
{
    struct ordered_vector_t vector;
};

LIGHTSHIP_PUBLIC_API struct map_t*
map_create(void);

LIGHTSHIP_PUBLIC_API void
map_init_map(struct map_t* map);

LIGHTSHIP_PUBLIC_API void
map_destroy(struct map_t* map);

LIGHTSHIP_PUBLIC_API void
map_insert(struct map_t* map, intptr_t hash, void* value);

LIGHTSHIP_PUBLIC_API void*
map_find(struct map_t* map, intptr_t hash);

LIGHTSHIP_PUBLIC_API void*
map_erase(struct map_t* map, intptr_t hash);

LIGHTSHIP_PUBLIC_API void
map_clear(struct map_t* map);

LIGHTSHIP_PUBLIC_API intptr_t
map_count(struct map_t* map);

#ifdef _DEBUG
void
map_print(struct map_t* map);
#endif

#define MAP_FOR_EACH(map, var_type, var) \
    intptr_t map_internal_i; \
    var_type* var; \
    for(map_internal_i = 0, var = ((struct map_key_value_t*)(map)->vector.data)[map_internal_i].value; \
        map_internal_i != (map)->vector.count; \
        ++map_internal_i, var = ((struct map_key_value_t*)(map)->vector.data)[map_internal_i].value)

#define MAP_ERASE_IN_FOR_LOOP(map, element) \
    ordered_vector_erase_element((map)->vector, element);

C_HEADER_END

#endif /* LIGHTSHIP_UTIL_MAP_H */
