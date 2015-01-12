#include "gmock/gmock.h"
#include "util/map.h"

#define NAME map

TEST(NAME, init)
{
    struct map_t map;
    map.vector.count = 4;
    map.vector.capacity = 56;
    map.vector.data = (DATA_POINTER_TYPE*)4783;
    map.vector.element_size = 283;
    
    map_init_map(&map);
    ASSERT_EQ(0, map.vector.count);
    
    ASSERT_EQ(0, map.vector.capacity);
    ASSERT_EQ(0, map.vector.count);
    ASSERT_EQ(NULL, map.vector.data);
    ASSERT_EQ(sizeof(struct map_key_value_t), map.vector.element_size);
}

TEST(NAME, create_initialises_map)
{
    struct map_t* map = map_create();
    ASSERT_EQ(0, map->vector.capacity);
    ASSERT_EQ(0, map->vector.count);
    ASSERT_EQ(NULL, map->vector.data);
    ASSERT_EQ(sizeof(struct map_key_value_t), map->vector.element_size);
    map_destroy(map);
}

TEST(NAME, insertion_forwards)
{
    struct map_t* map = map_create();
    
    int a=56, b=45, c=18, d=27, e=84;
    map_insert(map, 0, &a);
    map_insert(map, 1, &b);
    map_insert(map, 2, &c);
    map_insert(map, 3, &d);
    map_insert(map, 4, &e);
    
    ASSERT_EQ(a, *(int*)map_find(map, 0));
    ASSERT_EQ(b, *(int*)map_find(map, 1));
    ASSERT_EQ(c, *(int*)map_find(map, 2));
    ASSERT_EQ(d, *(int*)map_find(map, 3));
    ASSERT_EQ(e, *(int*)map_find(map, 4));
    
    map_destroy(map);
}

TEST(NAME, insertion_backwards)
{
    struct map_t* map = map_create();
    
    int a=56, b=45, c=18, d=27, e=84;
    map_insert(map, 4, &a);
    map_insert(map, 3, &b);
    map_insert(map, 2, &c);
    map_insert(map, 1, &d);
    map_insert(map, 0, &e);
    
    ASSERT_EQ(e, *(int*)map_find(map, 0));
    ASSERT_EQ(d, *(int*)map_find(map, 1));
    ASSERT_EQ(c, *(int*)map_find(map, 2));
    ASSERT_EQ(b, *(int*)map_find(map, 3));
    ASSERT_EQ(a, *(int*)map_find(map, 4));
    
    map_destroy(map);
}

TEST(NAME, insertion_random)
{
    struct map_t* map = map_create();
    
    int a=56, b=45, c=18, d=27, e=84;
    map_insert(map,26, &a);
    map_insert(map,44, &b);
    map_insert(map,82, &c);
    map_insert(map,41, &d);
    map_insert(map,70, &e);
    
    ASSERT_EQ(a, *(int*)map_find(map, 26));
    ASSERT_EQ(d, *(int*)map_find(map, 41));
    ASSERT_EQ(b, *(int*)map_find(map, 44));
    ASSERT_EQ(e, *(int*)map_find(map, 70));
    ASSERT_EQ(c, *(int*)map_find(map, 82));
    
    map_destroy(map);
}

TEST(NAME, erase_elements)
{
    struct map_t* map = map_create();
    
    int a=56, b=45, c=18, d=27, e=84;
    map_insert(map, 0, &a);
    map_insert(map, 1, &b);
    map_insert(map, 2, &c);
    map_insert(map, 3, &d);
    map_insert(map, 4, &e);
    
    ASSERT_EQ(c, *(int*)map_erase(map, 2));
    
    /* 4 */
    ASSERT_EQ(a, *(int*)map_find(map, 0));
    ASSERT_EQ(b, *(int*)map_find(map, 1));
    ASSERT_EQ(d, *(int*)map_find(map, 3));
    ASSERT_EQ(e, *(int*)map_find(map, 4));
    
    ASSERT_EQ(e, *(int*)map_erase(map, 4));
    
    /* 3 */
    ASSERT_EQ(a, *(int*)map_find(map, 0));
    ASSERT_EQ(b, *(int*)map_find(map, 1));
    ASSERT_EQ(d, *(int*)map_find(map, 3));
    
    ASSERT_EQ(a, *(int*)map_erase(map, 0));
    
    /* 2 */
    ASSERT_EQ(b, *(int*)map_find(map, 1));
    ASSERT_EQ(d, *(int*)map_find(map, 3));
    
    ASSERT_EQ(b, *(int*)map_erase(map, 1));
    
    /* 1 */
    ASSERT_EQ(d, *(int*)map_find(map, 3));
    
    ASSERT_EQ(d, *(int*)map_erase(map, 3));
    
    ASSERT_EQ(NULL, map_erase(map, 2));
    
    map_destroy(map);
}
