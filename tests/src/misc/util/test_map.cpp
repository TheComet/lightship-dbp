#include "gmock/gmock.h"
#include "util/map.h"

#define NAME map

using testing::NotNull;

TEST(NAME, init_sets_correct_values)
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
    map_insert(map, 26, &a);
    map_insert(map, 44, &b);
    map_insert(map, 82, &c);
    map_insert(map, 41, &d);
    map_insert(map, 70, &e);

    ASSERT_EQ(a, *(int*)map_find(map, 26));
    ASSERT_EQ(d, *(int*)map_find(map, 41));
    ASSERT_EQ(b, *(int*)map_find(map, 44));
    ASSERT_EQ(e, *(int*)map_find(map, 70));
    ASSERT_EQ(c, *(int*)map_find(map, 82));

    map_destroy(map);
}

TEST(NAME, clear_keeps_underlying_vector)
{
    struct map_t* map = map_create();

    int a = 53;
    map_insert(map, 0, &a);
    map_insert(map, 1, &a);
    map_insert(map, 2, &a);

    // this should delete all entries but keep the underlying vector
    map_clear(map);

    ASSERT_EQ(0, map->vector.count);
    EXPECT_THAT(map->vector.data, NotNull());

    map_destroy(map);
}

TEST(NAME, clear_free_deletes_underlying_vector)
{
    struct map_t* map = map_create();

    int a=53;
    map_insert(map, 0, &a);
    map_insert(map, 1, &a);
    map_insert(map, 2, &a);

    // this should delete all entries + free the underlying vector
    map_clear_free(map);

    ASSERT_EQ(0, map->vector.count);
    ASSERT_EQ(NULL, map->vector.data);

    map_destroy(map);
}

TEST(NAME, count_returns_correct_number)
{
    struct map_t* map = map_create();

    int a=53;
    map_insert(map, 0, &a);
    map_insert(map, 1, &a);
    map_insert(map, 2, &a);

    ASSERT_EQ(3, map_count(map));

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

    // 4
    ASSERT_EQ(a, *(int*)map_find(map, 0));
    ASSERT_EQ(b, *(int*)map_find(map, 1));
    ASSERT_EQ(d, *(int*)map_find(map, 3));
    ASSERT_EQ(e, *(int*)map_find(map, 4));

    ASSERT_EQ(e, *(int*)map_erase(map, 4));

    // 3
    ASSERT_EQ(a, *(int*)map_find(map, 0));
    ASSERT_EQ(b, *(int*)map_find(map, 1));
    ASSERT_EQ(d, *(int*)map_find(map, 3));

    ASSERT_EQ(a, *(int*)map_erase(map, 0));

    // 2
    ASSERT_EQ(b, *(int*)map_find(map, 1));
    ASSERT_EQ(d, *(int*)map_find(map, 3));

    ASSERT_EQ(b, *(int*)map_erase(map, 1));

    // 1
    ASSERT_EQ(d, *(int*)map_find(map, 3));

    ASSERT_EQ(d, *(int*)map_erase(map, 3));

    ASSERT_EQ(NULL, map_erase(map, 2));

    map_destroy(map);
}

TEST(NAME, reinsertion_forwards)
{
    struct map_t* map = map_create();

    int a=56, b=45, c=18, d=27, e=84;
    map_insert(map, 0, &a);
    map_insert(map, 1, &b);
    map_insert(map, 2, &c);
    map_insert(map, 3, &d);
    map_insert(map, 4, &e);

    map_erase(map, 4);
    map_erase(map, 3);
    map_erase(map, 2);

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

TEST(NAME, reinsertion_backwards)
{
    struct map_t* map = map_create();

    int a=56, b=45, c=18, d=27, e=84;
    map_insert(map, 4, &a);
    map_insert(map, 3, &b);
    map_insert(map, 2, &c);
    map_insert(map, 1, &d);
    map_insert(map, 0, &e);

    map_erase(map, 0);
    map_erase(map, 1);
    map_erase(map, 2);

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

TEST(NAME, reinsertion_random)
{
    struct map_t* map = map_create();

    int a=56, b=45, c=18, d=27, e=84;
    map_insert(map, 26, &a);
    map_insert(map, 44, &b);
    map_insert(map, 82, &c);
    map_insert(map, 41, &d);
    map_insert(map, 70, &e);

    map_erase(map, 44);
    map_erase(map, 70);
    map_erase(map, 26);

    map_insert(map, 26, &a);
    map_insert(map, 70, &e);
    map_insert(map, 44, &b);

    ASSERT_EQ(a, *(int*)map_find(map, 26));
    ASSERT_EQ(d, *(int*)map_find(map, 41));
    ASSERT_EQ(b, *(int*)map_find(map, 44));
    ASSERT_EQ(e, *(int*)map_find(map, 70));
    ASSERT_EQ(c, *(int*)map_find(map, 82));

    map_destroy(map);
}

TEST(NAME, inserting_duplicate_keys_doesnt_replace_existing_elements)
{
    struct map_t* map = map_create();

    int a=56, b=45, c=18;
    map_insert(map, 5, &a);
    map_insert(map, 3, &a);

    map_insert(map, 5, &b);
    map_insert(map, 4, &b);
    map_insert(map, 3, &c);

    ASSERT_EQ(a, *(int*)map_find(map, 3));
    ASSERT_EQ(b, *(int*)map_find(map, 4));
    ASSERT_EQ(a, *(int*)map_find(map, 5));

    map_destroy(map);
}

TEST(NAME, generating_keys_do_not_conflict_with_existing_ascending_keys)
{
    intptr_t key;
    struct map_t* map = map_create();
    map_insert(map, 0, NULL);
    map_insert(map, 1, NULL);
    map_insert(map, 2, NULL);
    map_insert(map, 3, NULL);
    map_insert(map, 5, NULL);
    key = map_find_unused_key(map);
    ASSERT_NE(0, key);
    ASSERT_NE(1, key);
    ASSERT_NE(2, key);
    ASSERT_NE(3, key);
    ASSERT_NE(5, key);
    map_destroy(map);
}

TEST(NAME, generating_keys_do_not_conflict_with_existing_descending_keys)
{
        intptr_t key;
    struct map_t* map = map_create();
    map_insert(map, 5, NULL);
    map_insert(map, 3, NULL);
    map_insert(map, 2, NULL);
    map_insert(map, 1, NULL);
    map_insert(map, 0, NULL);
    key = map_find_unused_key(map);
    ASSERT_NE(0, key);
    ASSERT_NE(1, key);
    ASSERT_NE(2, key);
    ASSERT_NE(3, key);
    ASSERT_NE(5, key);
    map_destroy(map);
}


TEST(NAME, generating_keys_do_not_conflict_with_existing_random_keys)
{
    intptr_t key;
    struct map_t* map = map_create();
    map_insert(map, 2387, NULL);
    map_insert(map, 28, NULL);
    map_insert(map, 358, NULL);
    map_insert(map, 183, NULL);
    map_insert(map, 38, NULL);
    key = map_find_unused_key(map);
    ASSERT_NE(2387, key);
    ASSERT_NE(28, key);
    ASSERT_NE(358, key);
    ASSERT_NE(183, key);
    ASSERT_NE(38, key);
    map_destroy(map);
}

TEST(NAME, iterate_with_no_items)
{
    struct map_t* map = map_create();
    {
        int counter = 0;
        MAP_FOR_EACH(map, int, key, value)
            ++counter;
        MAP_END_EACH
        ASSERT_EQ(0, counter);
    }
    map_destroy(map);
}

TEST(NAME, iterate_5_random_items)
{
    struct map_t* map = map_create();

    int a=79579, b=235, c=347, d=124, e=457;
    map_insert(map, 243, &a);
    map_insert(map, 256, &b);
    map_insert(map, 456, &c);
    map_insert(map, 468, &d);
    map_insert(map, 969, &e);

	int counter = 0;
	MAP_FOR_EACH(map, int, key, value)
		switch(counter)
		{
			case 0 : ASSERT_EQ(243, key); ASSERT_EQ(a, *value); break;
			case 1 : ASSERT_EQ(256, key); ASSERT_EQ(b, *value); break;
			case 2 : ASSERT_EQ(456, key); ASSERT_EQ(c, *value); break;
			case 3 : ASSERT_EQ(468, key); ASSERT_EQ(d, *value); break;
			case 4 : ASSERT_EQ(969, key); ASSERT_EQ(e, *value); break;
			default: ASSERT_EQ(0, 1); break;
		}
		++counter;
	MAP_END_EACH
	ASSERT_EQ(5, counter);

    map_destroy(map);
}

TEST(NAME, iterate_5_null_items)
{
    struct map_t* map = map_create();

    map_insert(map, 243, NULL);
    map_insert(map, 256, NULL);
    map_insert(map, 456, NULL);
    map_insert(map, 468, NULL);
    map_insert(map, 969, NULL);

	int counter = 0;
	MAP_FOR_EACH(map, int, key, value)
		switch(counter)
		{
			case 0 : ASSERT_EQ(243, key); ASSERT_EQ(NULL, value); break;
			case 1 : ASSERT_EQ(256, key); ASSERT_EQ(NULL, value); break;
			case 2 : ASSERT_EQ(456, key); ASSERT_EQ(NULL, value); break;
			case 3 : ASSERT_EQ(468, key); ASSERT_EQ(NULL, value); break;
			case 4 : ASSERT_EQ(969, key); ASSERT_EQ(NULL, value); break;
			default: ASSERT_EQ(0, 1); break;
		}
		++counter;
	MAP_END_EACH
	ASSERT_EQ(5, counter);

    map_destroy(map);
}
