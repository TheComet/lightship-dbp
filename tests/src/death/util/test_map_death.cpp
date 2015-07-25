#include "tests/globals.hpp"
#include "gmock/gmock.h"
#include "util/bst_vector.h"

#define NAME bstv_DeathTest

using namespace testing;

TEST(NAME, init_with_bstv_as_null_ptr)
{
    ASSERT_DEATH(bstv_init_bstv(NULL), ASSERTION_REGEX);
}

TEST(NAME, destroy_with_bstv_as_null_ptr)
{
    ASSERT_DEATH(bstv_destroy(NULL), ASSERTION_REGEX);
}

TEST(NAME, find_with_bstv_as_null_ptr)
{
    ASSERT_DEATH(bstv_find(NULL, 2394), ASSERTION_REGEX);
}

TEST(NAME, find_element_with_bstv_as_null_ptr)
{
    int a = 6;
    ASSERT_DEATH(bstv_find_element(NULL, &a), ASSERTION_REGEX);
}

TEST(NAME, key_exists_with_bstv_as_null_ptr)
{
    ASSERT_DEATH(bstv_hash_exists(NULL, 2435), ASSERTION_REGEX);
}

TEST(NAME, find_unused_key_with_bstv_as_null_ptr)
{
    ASSERT_DEATH(bstv_find_unused_hash(NULL), ASSERTION_REGEX);
}

TEST(NAME, insert_with_bstv_as_null_ptr)
{
    int a = 4;
    ASSERT_DEATH(bstv_insert(NULL, 235, &a), ASSERTION_REGEX);
}

TEST(NAME, set_with_bstv_as_null_ptr)
{
    int a = 6;
    ASSERT_DEATH(bstv_set(NULL, 246, &a), ASSERTION_REGEX);
}

TEST(NAME, erase_with_bstv_as_null_ptr)
{
    ASSERT_DEATH(bstv_erase(NULL, 235), ASSERTION_REGEX);
}

TEST(NAME, erase_element_with_bstv_as_null_ptr)
{
    int a = 6;
    ASSERT_DEATH(bstv_erase_element(NULL, &a), ASSERTION_REGEX);
}

TEST(NAME, clear_with_bstv_as_null_ptr)
{
    ASSERT_DEATH(bstv_clear(NULL), ASSERTION_REGEX);
}

TEST(NAME, clear_free_with_bstv_as_null_ptr)
{
    ASSERT_DEATH(bstv_clear_free(NULL), ASSERTION_REGEX);
}
