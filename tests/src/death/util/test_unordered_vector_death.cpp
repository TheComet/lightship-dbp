#include "tests/globals.hpp"
#include "gmock/gmock.h"
#include "util/unordered_vector.h"

#define NAME unordered_vector_DeathTest

using namespace testing;

TEST(NAME, init_with_vector_as_null_ptr)
{
    ASSERT_DEATH(unordered_vector_init_vector(NULL, sizeof(int)), ASSERTION_REGEX);
}

TEST(NAME, destroy_with_vector_as_null_ptr)
{
    ASSERT_DEATH(unordered_vector_destroy(NULL), ASSERTION_REGEX);
}

TEST(NAME, clear_with_vector_as_null_ptr)
{
    ASSERT_DEATH(unordered_vector_clear(NULL), ASSERTION_REGEX);
}

TEST(NAME, clear_free_with_vector_as_null_ptr)
{
    ASSERT_DEATH(unordered_vector_clear_free(NULL), ASSERTION_REGEX);
}

TEST(NAME, push_with_vector_as_null_ptr)
{
    int a = 6;
    ASSERT_DEATH(unordered_vector_push(NULL, &a), ASSERTION_REGEX);
}

TEST(NAME, push_with_data_as_null_ptr)
{
    unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    EXPECT_DEATH(unordered_vector_push(vec, NULL), ASSERTION_REGEX);
    unordered_vector_destroy(vec);
}

TEST(NAME, push_emplace_with_vector_as_null_ptr)
{
    int a = 6;
    ASSERT_DEATH(unordered_vector_push_emplace(NULL), ASSERTION_REGEX);
}

TEST(NAME, pop_with_vector_as_null_ptr)
{
    ASSERT_DEATH(unordered_vector_pop(NULL), ASSERTION_REGEX);
}

TEST(NAME, back_with_vector_as_null_ptr)
{
    ASSERT_DEATH(unordered_vector_back(NULL), ASSERTION_REGEX);
}

TEST(NAME, erase_index_with_vector_as_null_ptr)
{
    ASSERT_DEATH(unordered_vector_erase_index(NULL, 0), ASSERTION_REGEX);
}

TEST(NAME, erase_element_with_vector_as_null_ptr)
{
    int a = 6;
    ASSERT_DEATH(unordered_vector_erase_element(NULL, &a), ASSERTION_REGEX);
}

TEST(NAME, erase_element_with_element_as_null_ptr)
{
    unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    EXPECT_DEATH(unordered_vector_erase_element(vec, NULL), ASSERTION_REGEX);
    unordered_vector_destroy(vec);
}

TEST(NAME, erase_element_with_element_as_out_of_bounds_ptr)
{
    int* ptr;
    unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    ptr = (int*)unordered_vector_push_emplace(vec);
    EXPECT_DEATH(unordered_vector_erase_element(vec, ptr + 200), ".*Assertion.*");
    unordered_vector_destroy(vec);
}

TEST(NAME, get_element_with_vec_as_null_ptr)
{
    ASSERT_DEATH(unordered_vector_get_element(NULL, 0), ASSERTION_REGEX);
}
