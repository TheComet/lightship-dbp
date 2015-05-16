#include "tests/globals.hpp"
#include "gmock/gmock.h"
#include "util/map.h"

#define NAME map_DeathTest

using namespace testing;

TEST(NAME, init_with_map_as_null_ptr)
{
    ASSERT_DEATH(map_init_map(NULL), ASSERTION_REGEX);
}

TEST(NAME, destroy_with_map_as_null_ptr)
{
    ASSERT_DEATH(map_destroy(NULL), ASSERTION_REGEX);
}

TEST(NAME, find_with_map_as_null_ptr)
{
    ASSERT_DEATH(map_find(NULL, 2394), ASSERTION_REGEX);
}

TEST(NAME, find_element_with_map_as_null_ptr)
{
    int a = 6;
    ASSERT_DEATH(map_find_element(NULL, &a), ASSERTION_REGEX);
}

TEST(NAME, key_exists_with_map_as_null_ptr)
{
    ASSERT_DEATH(map_key_exists(NULL, 2435), ASSERTION_REGEX);
}

TEST(NAME, find_unused_key_with_map_as_null_ptr)
{
    ASSERT_DEATH(map_find_unused_key(NULL), ASSERTION_REGEX);
}

TEST(NAME, insert_with_map_as_null_ptr)
{
    int a = 4;
    ASSERT_DEATH(map_insert(NULL, 235, &a), ASSERTION_REGEX);
}

TEST(NAME, set_with_map_as_null_ptr)
{
    int a = 6;
    ASSERT_DEATH(map_set(NULL, 246, &a), ASSERTION_REGEX);
}

TEST(NAME, erase_with_map_as_null_ptr)
{
    ASSERT_DEATH(map_erase(NULL, 235), ASSERTION_REGEX);
}

TEST(NAME, erase_element_with_map_as_null_ptr)
{
    int a = 6;
    ASSERT_DEATH(map_erase_element(NULL, &a), ASSERTION_REGEX);
}

TEST(NAME, clear_with_map_as_null_ptr)
{
    ASSERT_DEATH(map_clear(NULL), ASSERTION_REGEX);
}

TEST(NAME, clear_free_with_map_as_null_ptr)
{
    ASSERT_DEATH(map_clear_free(NULL), ASSERTION_REGEX);
}
