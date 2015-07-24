#include "gmock/gmock.h"
#include "util/bst_hashed_vector.h"

#define NAME bsthv

using testing::NotNull;

TEST(NAME, init_sets_correct_values)
{
    struct bsthv_t bsthv;
    bsthv.vector.count = 4;
    bsthv.vector.capacity = 56;
    bsthv.vector.data = (DATA_POINTER_TYPE*)4783;
    bsthv.vector.element_size = 283;

    bsthv_init_bsthv(&bsthv);
    ASSERT_EQ(0, bsthv.vector.count);

    ASSERT_EQ(0, bsthv.vector.capacity);
    ASSERT_EQ(0, bsthv.vector.count);
    ASSERT_EQ(NULL, bsthv.vector.data);
    ASSERT_EQ(sizeof(struct bsthv_key_value_t), bsthv.vector.element_size);
}

TEST(NAME, create_initialises_bsthv)
{
    struct bsthv_t* bsthv = bsthv_create();
    ASSERT_EQ(0, bsthv->vector.capacity);
    ASSERT_EQ(0, bsthv->vector.count);
    ASSERT_EQ(NULL, bsthv->vector.data);
    ASSERT_EQ(sizeof(struct bsthv_key_value_t), bsthv->vector.element_size);
    bsthv_destroy(bsthv);
}

TEST(NAME, insertion_forwards)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bsthv_insert(bsthv, 0, &a);
    bsthv_insert(bsthv, 1, &b);
    bsthv_insert(bsthv, 2, &c);
    bsthv_insert(bsthv, 3, &d);
    bsthv_insert(bsthv, 4, &e);

    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, 0));
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, 1));
    ASSERT_EQ(c, *(int*)bsthv_find(bsthv, 2));
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, 3));
    ASSERT_EQ(e, *(int*)bsthv_find(bsthv, 4));

    bsthv_destroy(bsthv);
}

TEST(NAME, insertion_backwards)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bsthv_insert(bsthv, 4, &a);
    bsthv_insert(bsthv, 3, &b);
    bsthv_insert(bsthv, 2, &c);
    bsthv_insert(bsthv, 1, &d);
    bsthv_insert(bsthv, 0, &e);

    ASSERT_EQ(e, *(int*)bsthv_find(bsthv, 0));
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, 1));
    ASSERT_EQ(c, *(int*)bsthv_find(bsthv, 2));
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, 3));
    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, 4));

    bsthv_destroy(bsthv);
}

TEST(NAME, insertion_random)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bsthv_insert(bsthv, 26, &a);
    bsthv_insert(bsthv, 44, &b);
    bsthv_insert(bsthv, 82, &c);
    bsthv_insert(bsthv, 41, &d);
    bsthv_insert(bsthv, 70, &e);

    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, 26));
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, 41));
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, 44));
    ASSERT_EQ(e, *(int*)bsthv_find(bsthv, 70));
    ASSERT_EQ(c, *(int*)bsthv_find(bsthv, 82));

    bsthv_destroy(bsthv);
}

TEST(NAME, clear_keeps_underlying_vector)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a = 53;
    bsthv_insert(bsthv, 0, &a);
    bsthv_insert(bsthv, 1, &a);
    bsthv_insert(bsthv, 2, &a);

    // this should delete all entries but keep the underlying vector
    bsthv_clear(bsthv);

    ASSERT_EQ(0, bsthv->vector.count);
    EXPECT_THAT(bsthv->vector.data, NotNull());

    bsthv_destroy(bsthv);
}

TEST(NAME, clear_free_deletes_underlying_vector)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a=53;
    bsthv_insert(bsthv, 0, &a);
    bsthv_insert(bsthv, 1, &a);
    bsthv_insert(bsthv, 2, &a);

    // this should delete all entries + free the underlying vector
    bsthv_clear_free(bsthv);

    ASSERT_EQ(0, bsthv->vector.count);
    ASSERT_EQ(NULL, bsthv->vector.data);

    bsthv_destroy(bsthv);
}

TEST(NAME, count_returns_correct_number)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a=53;
    bsthv_insert(bsthv, 0, &a);
    bsthv_insert(bsthv, 1, &a);
    bsthv_insert(bsthv, 2, &a);

    ASSERT_EQ(3, bsthv_count(bsthv));

    bsthv_destroy(bsthv);
}

TEST(NAME, erase_elements)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bsthv_insert(bsthv, 0, &a);
    bsthv_insert(bsthv, 1, &b);
    bsthv_insert(bsthv, 2, &c);
    bsthv_insert(bsthv, 3, &d);
    bsthv_insert(bsthv, 4, &e);

    ASSERT_EQ(c, *(int*)bsthv_erase(bsthv, 2));

    // 4
    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, 0));
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, 1));
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, 3));
    ASSERT_EQ(e, *(int*)bsthv_find(bsthv, 4));

    ASSERT_EQ(e, *(int*)bsthv_erase(bsthv, 4));

    // 3
    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, 0));
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, 1));
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, 3));

    ASSERT_EQ(a, *(int*)bsthv_erase(bsthv, 0));

    // 2
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, 1));
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, 3));

    ASSERT_EQ(b, *(int*)bsthv_erase(bsthv, 1));

    // 1
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, 3));

    ASSERT_EQ(d, *(int*)bsthv_erase(bsthv, 3));

    ASSERT_EQ(NULL, bsthv_erase(bsthv, 2));

    bsthv_destroy(bsthv);
}

TEST(NAME, reinsertion_forwards)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bsthv_insert(bsthv, 0, &a);
    bsthv_insert(bsthv, 1, &b);
    bsthv_insert(bsthv, 2, &c);
    bsthv_insert(bsthv, 3, &d);
    bsthv_insert(bsthv, 4, &e);

    bsthv_erase(bsthv, 4);
    bsthv_erase(bsthv, 3);
    bsthv_erase(bsthv, 2);

    bsthv_insert(bsthv, 2, &c);
    bsthv_insert(bsthv, 3, &d);
    bsthv_insert(bsthv, 4, &e);

    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, 0));
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, 1));
    ASSERT_EQ(c, *(int*)bsthv_find(bsthv, 2));
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, 3));
    ASSERT_EQ(e, *(int*)bsthv_find(bsthv, 4));

    bsthv_destroy(bsthv);
}

TEST(NAME, reinsertion_backwards)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bsthv_insert(bsthv, 4, &a);
    bsthv_insert(bsthv, 3, &b);
    bsthv_insert(bsthv, 2, &c);
    bsthv_insert(bsthv, 1, &d);
    bsthv_insert(bsthv, 0, &e);

    bsthv_erase(bsthv, 0);
    bsthv_erase(bsthv, 1);
    bsthv_erase(bsthv, 2);

    bsthv_insert(bsthv, 2, &c);
    bsthv_insert(bsthv, 1, &d);
    bsthv_insert(bsthv, 0, &e);

    ASSERT_EQ(e, *(int*)bsthv_find(bsthv, 0));
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, 1));
    ASSERT_EQ(c, *(int*)bsthv_find(bsthv, 2));
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, 3));
    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, 4));

    bsthv_destroy(bsthv);
}

TEST(NAME, reinsertion_random)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bsthv_insert(bsthv, 26, &a);
    bsthv_insert(bsthv, 44, &b);
    bsthv_insert(bsthv, 82, &c);
    bsthv_insert(bsthv, 41, &d);
    bsthv_insert(bsthv, 70, &e);

    bsthv_erase(bsthv, 44);
    bsthv_erase(bsthv, 70);
    bsthv_erase(bsthv, 26);

    bsthv_insert(bsthv, 26, &a);
    bsthv_insert(bsthv, 70, &e);
    bsthv_insert(bsthv, 44, &b);

    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, 26));
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, 41));
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, 44));
    ASSERT_EQ(e, *(int*)bsthv_find(bsthv, 70));
    ASSERT_EQ(c, *(int*)bsthv_find(bsthv, 82));

    bsthv_destroy(bsthv);
}

TEST(NAME, inserting_duplicate_keys_doesnt_replace_existing_elements)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a=56, b=45, c=18;
    bsthv_insert(bsthv, 5, &a);
    bsthv_insert(bsthv, 3, &a);

    bsthv_insert(bsthv, 5, &b);
    bsthv_insert(bsthv, 4, &b);
    bsthv_insert(bsthv, 3, &c);

    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, 3));
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, 4));
    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, 5));

    bsthv_destroy(bsthv);
}

TEST(NAME, generating_keys_do_not_conflict_with_existing_ascending_keys)
{
    intptr_t key;
    struct bsthv_t* bsthv = bsthv_create();
    bsthv_insert(bsthv, 0, NULL);
    bsthv_insert(bsthv, 1, NULL);
    bsthv_insert(bsthv, 2, NULL);
    bsthv_insert(bsthv, 3, NULL);
    bsthv_insert(bsthv, 5, NULL);
    key = bsthv_find_unused_key(bsthv);
    ASSERT_NE(0, key);
    ASSERT_NE(1, key);
    ASSERT_NE(2, key);
    ASSERT_NE(3, key);
    ASSERT_NE(5, key);
    bsthv_destroy(bsthv);
}

TEST(NAME, generating_keys_do_not_conflict_with_existing_descending_keys)
{
        intptr_t key;
    struct bsthv_t* bsthv = bsthv_create();
    bsthv_insert(bsthv, 5, NULL);
    bsthv_insert(bsthv, 3, NULL);
    bsthv_insert(bsthv, 2, NULL);
    bsthv_insert(bsthv, 1, NULL);
    bsthv_insert(bsthv, 0, NULL);
    key = bsthv_find_unused_key(bsthv);
    ASSERT_NE(0, key);
    ASSERT_NE(1, key);
    ASSERT_NE(2, key);
    ASSERT_NE(3, key);
    ASSERT_NE(5, key);
    bsthv_destroy(bsthv);
}


TEST(NAME, generating_keys_do_not_conflict_with_existing_random_keys)
{
    intptr_t key;
    struct bsthv_t* bsthv = bsthv_create();
    bsthv_insert(bsthv, 2387, NULL);
    bsthv_insert(bsthv, 28, NULL);
    bsthv_insert(bsthv, 358, NULL);
    bsthv_insert(bsthv, 183, NULL);
    bsthv_insert(bsthv, 38, NULL);
    key = bsthv_find_unused_key(bsthv);
    ASSERT_NE(2387, key);
    ASSERT_NE(28, key);
    ASSERT_NE(358, key);
    ASSERT_NE(183, key);
    ASSERT_NE(38, key);
    bsthv_destroy(bsthv);
}

TEST(NAME, iterate_with_no_items)
{
    struct bsthv_t* bsthv = bsthv_create();
    {
        int counter = 0;
        MAP_FOR_EACH(bsthv, int, key, value)
            ++counter;
        MAP_END_EACH
        ASSERT_EQ(0, counter);
    }
    bsthv_destroy(bsthv);
}

TEST(NAME, iterate_5_random_items)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a=79579, b=235, c=347, d=124, e=457;
    bsthv_insert(bsthv, 243, &a);
    bsthv_insert(bsthv, 256, &b);
    bsthv_insert(bsthv, 456, &c);
    bsthv_insert(bsthv, 468, &d);
    bsthv_insert(bsthv, 969, &e);

	int counter = 0;
	MAP_FOR_EACH(bsthv, int, key, value)
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

    bsthv_destroy(bsthv);
}

TEST(NAME, iterate_5_null_items)
{
    struct bsthv_t* bsthv = bsthv_create();

    bsthv_insert(bsthv, 243, NULL);
    bsthv_insert(bsthv, 256, NULL);
    bsthv_insert(bsthv, 456, NULL);
    bsthv_insert(bsthv, 468, NULL);
    bsthv_insert(bsthv, 969, NULL);

	int counter = 0;
	MAP_FOR_EACH(bsthv, int, key, value)
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

    bsthv_destroy(bsthv);
}
