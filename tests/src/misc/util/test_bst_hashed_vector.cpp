#include "gmock/gmock.h"
#include "util/bst_hashed_vector.h"

#define NAME bst_keyed_vector

using namespace testing;

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
    bsthv_insert(bsthv, "0", &a);
    bsthv_insert(bsthv, "1", &b);
    bsthv_insert(bsthv, "2", &c);
    bsthv_insert(bsthv, "3", &d);
    bsthv_insert(bsthv, "4", &e);

    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, "0"));
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, "1"));
    ASSERT_EQ(c, *(int*)bsthv_find(bsthv, "2"));
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, "3"));
    ASSERT_EQ(e, *(int*)bsthv_find(bsthv, "4"));

    bsthv_destroy(bsthv);
}

TEST(NAME, insertion_backwards)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bsthv_insert(bsthv, "4", &a);
    bsthv_insert(bsthv, "3", &b);
    bsthv_insert(bsthv, "2", &c);
    bsthv_insert(bsthv, "1", &d);
    bsthv_insert(bsthv, "0", &e);

    ASSERT_EQ(e, *(int*)bsthv_find(bsthv, "0"));
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, "1"));
    ASSERT_EQ(c, *(int*)bsthv_find(bsthv, "2"));
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, "3"));
    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, "4"));

    bsthv_destroy(bsthv);
}

TEST(NAME, insertion_random)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bsthv_insert(bsthv, "26", &a);
    bsthv_insert(bsthv, "44", &b);
    bsthv_insert(bsthv, "82", &c);
    bsthv_insert(bsthv, "41", &d);
    bsthv_insert(bsthv, "70", &e);

    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, "26"));
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, "41"));
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, "44"));
    ASSERT_EQ(e, *(int*)bsthv_find(bsthv, "70"));
    ASSERT_EQ(c, *(int*)bsthv_find(bsthv, "82"));

    bsthv_destroy(bsthv);
}

TEST(NAME, clear_keeps_underlying_vector)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a = 53;
    bsthv_insert(bsthv, "0", &a);
    bsthv_insert(bsthv, "1", &a);
    bsthv_insert(bsthv, "2", &a);

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
    bsthv_insert(bsthv, "0", &a);
    bsthv_insert(bsthv, "1", &a);
    bsthv_insert(bsthv, "2", &a);

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
    bsthv_insert(bsthv, "0", &a);
    bsthv_insert(bsthv, "1", &a);
    bsthv_insert(bsthv, "2", &a);

    ASSERT_EQ(3, bsthv_count(bsthv));

    bsthv_destroy(bsthv);
}

TEST(NAME, erase_elements)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bsthv_insert(bsthv, "0", &a);
    bsthv_insert(bsthv, "1", &b);
    bsthv_insert(bsthv, "2", &c);
    bsthv_insert(bsthv, "3", &d);
    bsthv_insert(bsthv, "4", &e);

    ASSERT_EQ(c, *(int*)bsthv_erase(bsthv, "2"));

    //"4"
    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, "0"));
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, "1"));
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, "3"));
    ASSERT_EQ(e, *(int*)bsthv_find(bsthv, "4"));

    ASSERT_EQ(e, *(int*)bsthv_erase(bsthv, "4"));

    //"3"
    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, "0"));
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, "1"));
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, "3"));

    ASSERT_EQ(a, *(int*)bsthv_erase(bsthv, "0"));

    //"2"
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, "1"));
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, "3"));

    ASSERT_EQ(b, *(int*)bsthv_erase(bsthv, "1"));

    //"1"
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, "3"));

    ASSERT_EQ(d, *(int*)bsthv_erase(bsthv, "3"));

    ASSERT_EQ(NULL, bsthv_erase(bsthv, "2"));

    bsthv_destroy(bsthv);
}

TEST(NAME, reinsertion_forwards)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bsthv_insert(bsthv, "0", &a);
    bsthv_insert(bsthv, "1", &b);
    bsthv_insert(bsthv, "2", &c);
    bsthv_insert(bsthv, "3", &d);
    bsthv_insert(bsthv, "4", &e);

    bsthv_erase(bsthv, "4");
    bsthv_erase(bsthv, "3");
    bsthv_erase(bsthv, "2");

    bsthv_insert(bsthv, "2", &c);
    bsthv_insert(bsthv, "3", &d);
    bsthv_insert(bsthv, "4", &e);

    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, "0"));
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, "1"));
    ASSERT_EQ(c, *(int*)bsthv_find(bsthv, "2"));
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, "3"));
    ASSERT_EQ(e, *(int*)bsthv_find(bsthv, "4"));

    bsthv_destroy(bsthv);
}

TEST(NAME, reinsertion_backwards)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bsthv_insert(bsthv, "4", &a);
    bsthv_insert(bsthv, "3", &b);
    bsthv_insert(bsthv, "2", &c);
    bsthv_insert(bsthv, "1", &d);
    bsthv_insert(bsthv, "0", &e);

    bsthv_erase(bsthv, "0");
    bsthv_erase(bsthv, "1");
    bsthv_erase(bsthv, "2");

    bsthv_insert(bsthv, "2", &c);
    bsthv_insert(bsthv, "1", &d);
    bsthv_insert(bsthv, "0", &e);

    ASSERT_EQ(e, *(int*)bsthv_find(bsthv, "0"));
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, "1"));
    ASSERT_EQ(c, *(int*)bsthv_find(bsthv, "2"));
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, "3"));
    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, "4"));

    bsthv_destroy(bsthv);
}

TEST(NAME, reinsertion_random)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a=56, b=45, c=18, d=27, e=84;
    bsthv_insert(bsthv, "26", &a);
    bsthv_insert(bsthv, "44", &b);
    bsthv_insert(bsthv, "82", &c);
    bsthv_insert(bsthv, "41", &d);
    bsthv_insert(bsthv, "70", &e);

    bsthv_erase(bsthv, "44");
    bsthv_erase(bsthv, "70");
    bsthv_erase(bsthv, "26");

    bsthv_insert(bsthv, "26", &a);
    bsthv_insert(bsthv, "70", &e);
    bsthv_insert(bsthv, "44", &b);

    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, "26"));
    ASSERT_EQ(d, *(int*)bsthv_find(bsthv, "41"));
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, "44"));
    ASSERT_EQ(e, *(int*)bsthv_find(bsthv, "70"));
    ASSERT_EQ(c, *(int*)bsthv_find(bsthv, "82"));

    bsthv_destroy(bsthv);
}

TEST(NAME, inserting_duplicate_keys_doesnt_replace_existing_elements)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a=56, b=45, c=18;
    bsthv_insert(bsthv, "5", &a);
    bsthv_insert(bsthv, "3", &a);

    bsthv_insert(bsthv, "5", &b);
    bsthv_insert(bsthv, "4", &b);
    bsthv_insert(bsthv, "3", &c);

    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, "3"));
    ASSERT_EQ(b, *(int*)bsthv_find(bsthv, "4"));
    ASSERT_EQ(a, *(int*)bsthv_find(bsthv, "5"));

    bsthv_destroy(bsthv);
}

TEST(NAME, find_element)
{
	struct bsthv_t* bsthv = bsthv_create();
	int a = 6;
	bsthv_insert(bsthv, "2387", NULL);
    bsthv_insert(bsthv, "28", &a);
    bsthv_insert(bsthv, "358", NULL);
    bsthv_insert(bsthv, "183", NULL);
    bsthv_insert(bsthv, "38", NULL);

	EXPECT_THAT(bsthv_find_element(bsthv, &a), StrEq("28"));

	bsthv_destroy(bsthv);
}

TEST(NAME, set_value)
{
	struct bsthv_t* bsthv = bsthv_create();
	int a = 6;
	bsthv_insert(bsthv, "2387", NULL);
    bsthv_insert(bsthv, "28", NULL);
    bsthv_insert(bsthv, "358", NULL);
    bsthv_insert(bsthv, "183", NULL);
    bsthv_insert(bsthv, "38", NULL);

	bsthv_set(bsthv, "28", &a);

	EXPECT_THAT((int*)bsthv_find(bsthv, "28"), Pointee(a));

	bsthv_destroy(bsthv);
}

TEST(NAME, get_any_element)
{
	struct bsthv_t* bsthv = bsthv_create();
	int a = 6;

	EXPECT_THAT(bsthv_get_any_element(bsthv), IsNull());
	bsthv_insert(bsthv, "45", &a);
	EXPECT_THAT(bsthv_get_any_element(bsthv), NotNull());
	bsthv_erase(bsthv, "45");
	EXPECT_THAT(bsthv_get_any_element(bsthv), IsNull());

	bsthv_destroy(bsthv);
}

TEST(NAME, key_exists)
{
	struct bsthv_t* bsthv = bsthv_create();

	EXPECT_THAT(bsthv_key_exists(bsthv, "29"), Eq(0));
	bsthv_insert(bsthv, "29", NULL);
	EXPECT_THAT(bsthv_key_exists(bsthv, "29"), Ne(0));
	EXPECT_THAT(bsthv_key_exists(bsthv, "40"), Eq(0));
	bsthv_erase(bsthv, "29");
	EXPECT_THAT(bsthv_key_exists(bsthv, "29"), Eq(0));

	bsthv_destroy(bsthv);
}

TEST(NAME, erase_element)
{
	struct bsthv_t* bsthv = bsthv_create();
	int a = 6;

	EXPECT_THAT(bsthv_erase_element(bsthv, &a), IsNull());
	EXPECT_THAT(bsthv_erase_element(bsthv, NULL), IsNull());
	bsthv_insert(bsthv, "39", &a);
	EXPECT_THAT((int*)bsthv_erase_element(bsthv, &a), Pointee(a));

	bsthv_destroy(bsthv);
}

TEST(NAME, iterate_with_no_items)
{
    struct bsthv_t* bsthv = bsthv_create();
    {
        int counter = 0;
        BSTHV_FOR_EACH(bsthv, int, key, value)
            ++counter;
        BSTHV_END_EACH
        ASSERT_EQ(0, counter);
    }
    bsthv_destroy(bsthv);
}

TEST(NAME, iterate_5_random_items)
{
    struct bsthv_t* bsthv = bsthv_create();

    int a=79579, b=235, c=347, d=124, e=457;
    bsthv_insert(bsthv, "243", &a);
    bsthv_insert(bsthv, "256", &b);
    bsthv_insert(bsthv, "456", &c);
    bsthv_insert(bsthv, "468", &d);
    bsthv_insert(bsthv, "969", &e);

	int counter = 0;
	BSTHV_FOR_EACH(bsthv, int, key, value)
		EXPECT_THAT(key, AnyOf(StrEq("243"), StrEq("256"), StrEq("456"), StrEq("468"), StrEq("969")));
		EXPECT_THAT(value, AnyOf(Pointee(a), Pointee(b), Pointee(c), Pointee(d), Pointee(e)));
		++counter;
	BSTHV_END_EACH
	EXPECT_THAT(counter, Eq(5));

    bsthv_destroy(bsthv);
}

TEST(NAME, iterate_5_null_items)
{
    struct bsthv_t* bsthv = bsthv_create();

    bsthv_insert(bsthv, "243", NULL);
    bsthv_insert(bsthv, "256", NULL);
    bsthv_insert(bsthv, "456", NULL);
    bsthv_insert(bsthv, "468", NULL);
    bsthv_insert(bsthv, "969", NULL);

	int counter = 0;
	BSTHV_FOR_EACH(bsthv, int, key, value)
		EXPECT_THAT(key, AnyOf(StrEq("243"), StrEq("256"), StrEq("456"), StrEq("468"), StrEq("969")));
		EXPECT_THAT(value, IsNull());
		++counter;
	BSTHV_END_EACH
	EXPECT_THAT(counter, Eq(5));

    bsthv_destroy(bsthv);
}

TEST(NAME, erase_in_for_loop)
{
	struct bsthv_t* bsthv = bsthv_create();

    int a=79579, b=235, c=347, d=124, e=457;
    bsthv_insert(bsthv, "243", &a);
    bsthv_insert(bsthv, "256", &b);
    bsthv_insert(bsthv, "456", &c);
    bsthv_insert(bsthv, "468", &d);
    bsthv_insert(bsthv, "969", &e);

	BSTHV_FOR_EACH(bsthv, int, key, value)
		if(strcmp(key, "256") == 0)
			BSTHV_ERASE_CURRENT_ITEM_IN_FOR_LOOP(bsthv, key, value);
	BSTHV_END_EACH

	EXPECT_THAT((int*)bsthv_find(bsthv, "243"), Pointee(a));
	EXPECT_THAT(bsthv_find(bsthv, "256"), IsNull());
	EXPECT_THAT((int*)bsthv_find(bsthv, "456"), Pointee(c));
	EXPECT_THAT((int*)bsthv_find(bsthv, "468"), Pointee(d));
	EXPECT_THAT((int*)bsthv_find(bsthv, "969"), Pointee(e));

	bsthv_destroy(bsthv);
}
