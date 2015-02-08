#include "util/map.h"
#include "util/memory.h"

const uint32_t MAP_INVALID_KEY = -1;

/* ------------------------------------------------------------------------- */
struct map_t*
map_create(void)
{
    struct map_t* map = (struct map_t*)MALLOC(sizeof(struct map_t));
    map_init_map(map);
    return map;
}

/* ------------------------------------------------------------------------- */
void
map_init_map(struct map_t* map)
{
    ordered_vector_init_vector(&map->vector, sizeof(struct map_key_value_t));
}

/* ------------------------------------------------------------------------- */
void
map_destroy(struct map_t* map)
{
    map_clear(map);
    FREE(map);
}

/* ------------------------------------------------------------------------- */
struct map_key_value_t*
map_find_lower_bound(const struct map_t* map, uint32_t hash)
{
    uint32_t half;
    struct map_key_value_t* middle;
    struct map_key_value_t* data = (struct map_key_value_t*)map->vector.data;
    uint32_t len = map->vector.count;

    /* if the vector has no data, return NULL */
    if(!len)
        return NULL;

    /* algorithm taken from GNU GCC stdlibc++'s lower_bound function, line 2121 in stl_algo.h */
    /* https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.3/a02014.html */
    while(len > 0)
    {
        half = len >> 1;
        middle = data + half;
        if(middle->hash < hash)
        {
            data = middle;
            ++data;
            len = len - half - 1;
        }
        else
            len = half;
    }
    
    /* if "data" is pointing outside of the valid elements in the vector, also return NULL */
    if((intptr_t)data >= (intptr_t)map->vector.data + map->vector.count * map->vector.element_size)
        return NULL;
    else
        return data;
}

/* ------------------------------------------------------------------------- */
void*
map_find(const struct map_t* map, uint32_t hash)
{
    struct map_key_value_t* data = map_find_lower_bound(map, hash);
    if(!data || data->hash != hash)
        return NULL;
    return data->value;
}

/* ------------------------------------------------------------------------- */
uint32_t
map_find_element(struct map_t* map, void* value)
{
    ORDERED_VECTOR_FOR_EACH(&map->vector, struct map_key_value_t, kv)
    {
        if(kv->value == value)
            return kv->hash;
    }
    return MAP_INVALID_KEY;
}

/* ------------------------------------------------------------------------- */
char
map_key_exists(struct map_t* map, uint32_t hash)
{
    struct map_key_value_t* data = map_find_lower_bound(map, hash);
    if(data && data->hash == hash)
        return 1;
    return 0;
}

/* ------------------------------------------------------------------------- */
uint32_t
map_find_unused_key(struct map_t* map)
{
    uint32_t i = 0;
    
    MAP_FOR_EACH(map, void, key, value)
    {
        if(i != key)
            break;
        ++i;
    }
    return i;
}

/* ------------------------------------------------------------------------- */
char
map_insert(struct map_t* map, uint32_t hash, void* value)
{
    struct map_key_value_t* emplaced_data;
    struct map_key_value_t* data;
    
    /* don't insert reserved hashes */
    if(hash == MAP_INVALID_KEY)
        return 0;
    
    /* lookup location in map to insert */
    data = map_find_lower_bound(map, hash);
    if(data && data->hash == hash)
        return 0;

    /* either push back or insert, depending on whether there is already data
     * in the map */
    if(!data)
        emplaced_data = (struct map_key_value_t*)ordered_vector_push_emplace(&map->vector);
    else
        emplaced_data = ordered_vector_insert_emplace(&map->vector,
                          data - (struct map_key_value_t*)map->vector.data);
    
    emplaced_data->hash = hash;
    emplaced_data->value = value;
    
    return 1;
}

/* ------------------------------------------------------------------------- */
void
map_set(struct map_t* map, uint32_t hash, void* value)
{
    struct map_key_value_t* data = map_find_lower_bound(map, hash);
    if(data && data->hash == hash)
        data->value = value;
}

/* ------------------------------------------------------------------------- */
void*
map_erase(struct map_t* map, uint32_t hash)
{
    void* value;
    struct map_key_value_t* data = map_find_lower_bound(map, hash);
    if(!data || data->hash != hash)
        return NULL;
    
    value = data->value;
    ordered_vector_erase_element(&map->vector, (DATA_POINTER_TYPE*)data);
    return value;
}

/* ------------------------------------------------------------------------- */
void*
map_erase_element(struct map_t* map, void* value)
{
    void* data;
    uint32_t hash = map_find_element(map, value);
    if(hash == MAP_INVALID_KEY)
        return NULL;

    data = map_find_lower_bound(map, hash);
    ordered_vector_erase_element(&map->vector, (DATA_POINTER_TYPE*)data);
    
    return value;
}

/* ------------------------------------------------------------------------- */
void
map_clear(struct map_t* map)
{
    ordered_vector_clear(&map->vector);
}

/* ------------------------------------------------------------------------- */
void map_clear_free(struct map_t* map)
{
    ordered_vector_clear_free(&map->vector);
}

/* ------------------------------------------------------------------------- */
uint32_t
map_count(struct map_t* map)
{
    return map->vector.count;
}

/* ------------------------------------------------------------------------- */
#ifdef _DEBUG
#include <stdio.h>
void
map_print(struct map_t* map)
{
    int i = 0;
    {
        ORDERED_VECTOR_FOR_EACH(&map->vector, struct map_key_value_t, item)
        {
            printf("hash: %d, value (ptr): %p\n", item->hash, (void*)item->value);
            i++;
        }
        printf("items in map: %d\n", i);
    }
}
#endif
