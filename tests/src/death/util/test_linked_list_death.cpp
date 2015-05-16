#include "tests/globals.hpp"
#include "gmock/gmock.h"
#include "util/linked_list.h"

#define NAME linked_list_DeathTest

using namespace testing;

TEST(NAME, init_list_null_ptr)
{
    ASSERT_DEATH(list_init_list(NULL), ASSERTION_REGEX);
}

TEST(NAME, destroy_null_ptr)
{
    ASSERT_DEATH(list_destroy(NULL), ASSERTION_REGEX);
}

TEST(NAME, clear_null_ptr)
{
    ASSERT_DEATH(list_clear(NULL), ASSERTION_REGEX);
}

TEST(NAME, push_list_is_null_ptr)
{
    int a = 6;
    ASSERT_DEATH(list_push(NULL, &a), ASSERTION_REGEX);
}

TEST(NAME, list_pop_null_ptr)
{
    ASSERT_DEATH(list_pop(NULL), ASSERTION_REGEX);
}

TEST(NAME, erase_node_with_list_as_null_ptr)
{
    int a = 6;
    struct list_t* list = list_create();
    struct list_node_t* node = list_push(list, &a);

    EXPECT_DEATH(list_erase_node(NULL, node), ASSERTION_REGEX);

    list_destroy(list);
}

TEST(NAME, erase_node_with_node_as_null_ptr)
{
    int a = 6;
    struct list_t* list = list_create();
    struct list_node_t* node = list_push(list, &a);

    EXPECT_DEATH(list_erase_node(list, NULL), ASSERTION_REGEX);

    list_destroy(list);
}

TEST(NAME, erase_element_with_list_as_null_ptr)
{
    int a = 6;
    ASSERT_DEATH(list_erase_element(NULL, &a), ASSERTION_REGEX);
}
