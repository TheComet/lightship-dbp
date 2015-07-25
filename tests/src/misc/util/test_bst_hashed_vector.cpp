#include "gmock/gmock.h"
#include "util/bst_vector.h"

#define NAME bsthv

using testing::NotNull;

TEST(NAME, init_sets_correct_values)
{
    struct bstv_t bstv;
    bstv.vector.count = 4;
    bstv.vector.capacity = 56;
    bstv.vector.data = (DATA_POINTER_TYPE*)4783;
    bstv.vector.element_size = 283;

    bstv_init_bstv(&bstv);
    ASSERT_EQ(0, bstv.vector.count);

    ASSERT_EQ(0, bstv.vector.capacity);
    ASSERT_EQ(0, bstv.vector.count);
    ASSERT_EQ(NULL, bstv.vector.data);
    ASSERT_EQ(sizeof(struct bstv_key_value_t), bstv.vector.element_size);
}

TEST(NAME, create_initialises_bstv)
{
    struct bstv_t* bstv = bstv_create();
    ASSERT_EQ(0, bstv->vector.capacity);
    ASSERT_EQ(0, bstv->vector.count);
    ASSERT_EQ(NULL, bstv->vector.data);
    ASSERT_EQ(sizeof(struct bstv_key_value_t), bstv->vector.element_size);
    bstv_destroy(bstv);
}

TEST(NAME, insertion_forwards)
{
    struct bstv_t* bstv = bstv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bstv_insert(bstv, 0, &a);
    bstv_insert(bstv, 1, &b);
    bstv_insert(bstv, 2, &c);
    bstv_insert(bstv, 3, &d);
    bstv_insert(bstv, 4, &e);

    ASSERT_EQ(a, *(int*)bstv_find(bstv, 0));
    ASSERT_EQ(b, *(int*)bstv_find(bstv, 1));
    ASSERT_EQ(c, *(int*)bstv_find(bstv, 2));
    ASSERT_EQ(d, *(int*)bstv_find(bstv, 3));
    ASSERT_EQ(e, *(int*)bstv_find(bstv, 4));

    bstv_destroy(bstv);
}

TEST(NAME, insertion_backwards)
{
    struct bstv_t* bstv = bstv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bstv_insert(bstv, 4, &a);
    bstv_insert(bstv, 3, &b);
    bstv_insert(bstv, 2, &c);
    bstv_insert(bstv, 1, &d);
    bstv_insert(bstv, 0, &e);

    ASSERT_EQ(e, *(int*)bstv_find(bstv, 0));
    ASSERT_EQ(d, *(int*)bstv_find(bstv, 1));
    ASSERT_EQ(c, *(int*)bstv_find(bstv, 2));
    ASSERT_EQ(b, *(int*)bstv_find(bstv, 3));
    ASSERT_EQ(a, *(int*)bstv_find(bstv, 4));

    bstv_destroy(bstv);
}

TEST(NAME, insertion_random)
{
    struct bstv_t* bstv = bstv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bstv_insert(bstv, 26, &a);
    bstv_insert(bstv, 44, &b);
    bstv_insert(bstv, 82, &c);
    bstv_insert(bstv, 41, &d);
    bstv_insert(bstv, 70, &e);

    ASSERT_EQ(a, *(int*)bstv_find(bstv, 26));
    ASSERT_EQ(d, *(int*)bstv_find(bstv, 41));
    ASSERT_EQ(b, *(int*)bstv_find(bstv, 44));
    ASSERT_EQ(e, *(int*)bstv_find(bstv, 70));
    ASSERT_EQ(c, *(int*)bstv_find(bstv, 82));

    bstv_destroy(bstv);
}

TEST(NAME, clear_keeps_underlying_vector)
{
    struct bstv_t* bstv = bstv_create();

    int a = 53;
    bstv_insert(bstv, 0, &a);
    bstv_insert(bstv, 1, &a);
    bstv_insert(bstv, 2, &a);

    // this should delete all entries but keep the underlying vector
    bstv_clear(bstv);

    ASSERT_EQ(0, bstv->vector.count);
    EXPECT_THAT(bstv->vector.data, NotNull());

    bstv_destroy(bstv);
}

TEST(NAME, clear_free_deletes_underlying_vector)
{
    struct bstv_t* bstv = bstv_create();

    int a=53;
    bstv_insert(bstv, 0, &a);
    bstv_insert(bstv, 1, &a);
    bstv_insert(bstv, 2, &a);

    // this should delete all entries + free the underlying vector
    bstv_clear_free(bstv);

    ASSERT_EQ(0, bstv->vector.count);
    ASSERT_EQ(NULL, bstv->vector.data);

    bstv_destroy(bstv);
}

TEST(NAME, count_returns_correct_number)
{
    struct bstv_t* bstv = bstv_create();

    int a=53;
    bstv_insert(bstv, 0, &a);
    bstv_insert(bstv, 1, &a);
    bstv_insert(bstv, 2, &a);

    ASSERT_EQ(3, bstv_count(bstv));

    bstv_destroy(bstv);
}

TEST(NAME, erase_elements)
{
    struct bstv_t* bstv = bstv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bstv_insert(bstv, 0, &a);
    bstv_insert(bstv, 1, &b);
    bstv_insert(bstv, 2, &c);
    bstv_insert(bstv, 3, &d);
    bstv_insert(bstv, 4, &e);

    ASSERT_EQ(c, *(int*)bstv_erase(bstv, 2));

    // 4
    ASSERT_EQ(a, *(int*)bstv_find(bstv, 0));
    ASSERT_EQ(b, *(int*)bstv_find(bstv, 1));
    ASSERT_EQ(d, *(int*)bstv_find(bstv, 3));
    ASSERT_EQ(e, *(int*)bstv_find(bstv, 4));

    ASSERT_EQ(e, *(int*)bstv_erase(bstv, 4));

    // 3
    ASSERT_EQ(a, *(int*)bstv_find(bstv, 0));
    ASSERT_EQ(b, *(int*)bstv_find(bstv, 1));
    ASSERT_EQ(d, *(int*)bstv_find(bstv, 3));

    ASSERT_EQ(a, *(int*)bstv_erase(bstv, 0));

    // 2
    ASSERT_EQ(b, *(int*)bstv_find(bstv, 1));
    ASSERT_EQ(d, *(int*)bstv_find(bstv, 3));

    ASSERT_EQ(b, *(int*)bstv_erase(bstv, 1));

    // 1
    ASSERT_EQ(d, *(int*)bstv_find(bstv, 3));

    ASSERT_EQ(d, *(int*)bstv_erase(bstv, 3));

    ASSERT_EQ(NULL, bstv_erase(bstv, 2));

    bstv_destroy(bstv);
}

TEST(NAME, reinsertion_forwards)
{
    struct bstv_t* bstv = bstv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bstv_insert(bstv, 0, &a);
    bstv_insert(bstv, 1, &b);
    bstv_insert(bstv, 2, &c);
    bstv_insert(bstv, 3, &d);
    bstv_insert(bstv, 4, &e);

    bstv_erase(bstv, 4);
    bstv_erase(bstv, 3);
    bstv_erase(bstv, 2);

    bstv_insert(bstv, 2, &c);
    bstv_insert(bstv, 3, &d);
    bstv_insert(bstv, 4, &e);

    ASSERT_EQ(a, *(int*)bstv_find(bstv, 0));
    ASSERT_EQ(b, *(int*)bstv_find(bstv, 1));
    ASSERT_EQ(c, *(int*)bstv_find(bstv, 2));
    ASSERT_EQ(d, *(int*)bstv_find(bstv, 3));
    ASSERT_EQ(e, *(int*)bstv_find(bstv, 4));

    bstv_destroy(bstv);
}

TEST(NAME, reinsertion_backwards)
{
    struct bstv_t* bstv = bstv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bstv_insert(bstv, 4, &a);
    bstv_insert(bstv, 3, &b);
    bstv_insert(bstv, 2, &c);
    bstv_insert(bstv, 1, &d);
    bstv_insert(bstv, 0, &e);

    bstv_erase(bstv, 0);
    bstv_erase(bstv, 1);
    bstv_erase(bstv, 2);

    bstv_insert(bstv, 2, &c);
    bstv_insert(bstv, 1, &d);
    bstv_insert(bstv, 0, &e);

    ASSERT_EQ(e, *(int*)bstv_find(bstv, 0));
    ASSERT_EQ(d, *(int*)bstv_find(bstv, 1));
    ASSERT_EQ(c, *(int*)bstv_find(bstv, 2));
    ASSERT_EQ(b, *(int*)bstv_find(bstv, 3));
    ASSERT_EQ(a, *(int*)bstv_find(bstv, 4));

    bstv_destroy(bstv);
}

TEST(NAME, reinsertion_random)
{
    struct bstv_t* bstv = bstv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bstv_insert(bstv, 26, &a);
    bstv_insert(bstv, 44, &b);
    bstv_insert(bstv, 82, &c);
    bstv_insert(bstv, 41, &d);
    bstv_insert(bstv, 70, &e);

    bstv_erase(bstv, 44);
    bstv_erase(bstv, 70);
    bstv_erase(bstv, 26);

    bstv_insert(bstv, 26, &a);
    bstv_insert(bstv, 70, &e);
    bstv_insert(bstv, 44, &b);

    ASSERT_EQ(a, *(int*)bstv_find(bstv, 26));
    ASSERT_EQ(d, *(int*)bstv_find(bstv, 41));
    ASSERT_EQ(b, *(int*)bstv_find(bstv, 44));
    ASSERT_EQ(e, *(int*)bstv_find(bstv, 70));
    ASSERT_EQ(c, *(int*)bstv_find(bstv, 82));

    bstv_destroy(bstv);
}

TEST(NAME, inserting_duplicate_keys_doesnt_replace_existing_elements)
{
    struct bstv_t* bstv = bstv_create();

    int a=56, b=45, c=18;
    bstv_insert(bstv, 5, &a);
    bstv_insert(bstv, 3, &a);

    bstv_insert(bstv, 5, &b);
    bstv_insert(bstv, 4, &b);
    bstv_insert(bstv, 3, &c);

    ASSERT_EQ(a, *(int*)bstv_find(bstv, 3));
    ASSERT_EQ(b, *(int*)bstv_find(bstv, 4));
    ASSERT_EQ(a, *(int*)bstv_find(bstv, 5));

    bstv_destroy(bstv);
}

TEST(NAME, generating_keys_do_not_conflict_with_existing_ascending_keys)
{
    intptr_t key;
    struct bstv_t* bstv = bstv_create();
    bstv_insert(bstv, 0, NULL);
    bstv_insert(bstv, 1, NULL);
    bstv_insert(bstv, 2, NULL);
    bstv_insert(bstv, 3, NULL);
    bstv_insert(bstv, 5, NULL);
    key = bstv_find_unused_hash(bstv);
    ASSERT_NE(0, key);
    ASSERT_NE(1, key);
    ASSERT_NE(2, key);
    ASSERT_NE(3, key);
    ASSERT_NE(5, key);
    bstv_destroy(bstv);
}

TEST(NAME, generating_keys_do_not_conflict_with_existing_descending_keys)
{
        intptr_t key;
    struct bstv_t* bstv = bstv_create();
    bstv_insert(bstv, 5, NULL);
    bstv_insert(bstv, 3, NULL);
    bstv_insert(bstv, 2, NULL);
    bstv_insert(bstv, 1, NULL);
    bstv_insert(bstv, 0, NULL);
    key = bstv_find_unused_hash(bstv);
    ASSERT_NE(0, key);
    ASSERT_NE(1, key);
    ASSERT_NE(2, key);
    ASSERT_NE(3, key);
    ASSERT_NE(5, key);
    bstv_destroy(bstv);
}


TEST(NAME, generating_keys_do_not_conflict_with_existing_random_keys)
{
    intptr_t key;
    struct bstv_t* bstv = bstv_create();
    bstv_insert(bstv, 2387, NULL);
    bstv_insert(bstv, 28, NULL);
    bstv_insert(bstv, 358, NULL);
    bstv_insert(bstv, 183, NULL);
    bstv_insert(bstv, 38, NULL);
    key = bstv_find_unused_hash(bstv);
    ASSERT_NE(2387, key);
    ASSERT_NE(28, key);
    ASSERT_NE(358, key);
    ASSERT_NE(183, key);
    ASSERT_NE(38, key);
    bstv_destroy(bstv);
}

TEST(NAME, iterate_with_no_items)
{
    struct bstv_t* bstv = bstv_create();
    {
        int counter = 0;
        BSTV_FOR_EACH(bstv, int, key, value)
            ++counter;
        BSTV_END_EACH
        ASSERT_EQ(0, counter);
    }
    bstv_destroy(bstv);
}

TEST(NAME, iterate_5_random_items)
{
    struct bstv_t* bstv = bstv_create();

    int a=79579, b=235, c=347, d=124, e=457;
    bstv_insert(bstv, 243, &a);
    bstv_insert(bstv, 256, &b);
    bstv_insert(bstv, 456, &c);
    bstv_insert(bstv, 468, &d);
    bstv_insert(bstv, 969, &e);

	int counter = 0;
	BSTV_FOR_EACH(bstv, int, key, value)
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
	BSTV_END_EACH
	ASSERT_EQ(5, counter);

    bstv_destroy(bstv);
}

TEST(NAME, iterate_5_null_items)
{
    struct bstv_t* bstv = bstv_create();

    bstv_insert(bstv, 243, NULL);
    bstv_insert(bstv, 256, NULL);
    bstv_insert(bstv, 456, NULL);
    bstv_insert(bstv, 468, NULL);
    bstv_insert(bstv, 969, NULL);

	int counter = 0;
	BSTV_FOR_EACH(bstv, int, key, value)
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
	BSTV_END_EACH
	ASSERT_EQ(5, counter);

    bstv_destroy(bstv);
}
