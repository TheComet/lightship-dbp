#include "gmock/gmock.h"
#include "util/unordered_vector.h"

#define NAME unordered_vector

TEST(NAME, init)
{
    struct unordered_vector_t vec;
    
    vec.capacity = 45;
    vec.count = 384;
    vec.data = (DATA_POINTER_TYPE*)4859;
    vec.element_size = 183;
    unordered_vector_init_vector(&vec, sizeof(int));
    
    ASSERT_EQ(0, vec.capacity);
    ASSERT_EQ(0, vec.count);
    ASSERT_EQ(NULL, vec.data);
    ASSERT_EQ(sizeof(int), vec.element_size);
}

TEST(NAME, create_initialises_vector)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    ASSERT_EQ(0, vec->capacity);
    ASSERT_EQ(0, vec->count);
    ASSERT_EQ(NULL, vec->data);
    ASSERT_EQ(sizeof(int), vec->element_size);
    unordered_vector_destroy(vec);
}

TEST(NAME, push_increments_count_and_causes_realloc_by_factor_2)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    int x = 9;
    unordered_vector_push(vec, &x);
    ASSERT_EQ(2, vec->capacity);
    ASSERT_EQ(1, vec->count);
    unordered_vector_push(vec, &x);
    ASSERT_EQ(2, vec->capacity);
    ASSERT_EQ(2, vec->count);
    unordered_vector_push(vec, &x);
    ASSERT_EQ(4, vec->capacity);
    ASSERT_EQ(3, vec->count);
    unordered_vector_destroy(vec);
}

TEST(NAME, clear_keeps_buffer_and_resets_count)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    int x = 9;
    unordered_vector_push(vec, &x);
    unordered_vector_clear(vec);
    ASSERT_EQ(0, vec->count);
    ASSERT_EQ(2, vec->capacity);
    ASSERT_NE((void*)0, vec->data);
    unordered_vector_destroy(vec);
}

TEST(NAME, clear_free_deletes_buffer_and_resets_count)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    int x = 9;
    unordered_vector_push(vec, &x);
    unordered_vector_clear_free(vec);
    ASSERT_EQ(0, vec->count);
    ASSERT_EQ(0, vec->capacity);
    ASSERT_EQ(NULL, vec->data);
    unordered_vector_destroy(vec);
}

TEST(NAME, push_emplace_increments_count_and_causes_realloc_by_factor_2)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    unordered_vector_push_emplace(vec);
    ASSERT_EQ(2, vec->capacity);
    ASSERT_EQ(1, vec->count);
    unordered_vector_push_emplace(vec);
    ASSERT_EQ(2, vec->capacity);
    ASSERT_EQ(2, vec->count);
    unordered_vector_push_emplace(vec);
    ASSERT_EQ(4, vec->capacity);
    ASSERT_EQ(3, vec->count);
    unordered_vector_destroy(vec);
}

TEST(NAME, pop_returns_pushed_values)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    int x;
    x = 3; unordered_vector_push(vec, &x);
    x = 2; unordered_vector_push(vec, &x);
    x = 6; unordered_vector_push(vec, &x);
    ASSERT_EQ(6, *(int*)unordered_vector_pop(vec));
    x = 23; unordered_vector_push(vec, &x);
    x = 21; unordered_vector_push(vec, &x);
    ASSERT_EQ(21, *(int*)unordered_vector_pop(vec));
    ASSERT_EQ(23, *(int*)unordered_vector_pop(vec));
    ASSERT_EQ(2, *(int*)unordered_vector_pop(vec));
    ASSERT_EQ(3, *(int*)unordered_vector_pop(vec));
    ASSERT_EQ(0, vec->count);
    ASSERT_EQ(4, vec->capacity);
    ASSERT_NE((void*)0, vec->data);
    unordered_vector_destroy(vec);
}

TEST(NAME, pop_returns_push_emplaced_values)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    *(int*)unordered_vector_push_emplace(vec) = 53;
    *(int*)unordered_vector_push_emplace(vec) = 24;
    *(int*)unordered_vector_push_emplace(vec) = 73;
    ASSERT_EQ(73, *(int*)unordered_vector_pop(vec));
    *(int*)unordered_vector_push_emplace(vec) = 28;
    *(int*)unordered_vector_push_emplace(vec) = 72;
    ASSERT_EQ(72, *(int*)unordered_vector_pop(vec));
    ASSERT_EQ(28, *(int*)unordered_vector_pop(vec));
    ASSERT_EQ(24, *(int*)unordered_vector_pop(vec));
    ASSERT_EQ(53, *(int*)unordered_vector_pop(vec));
    ASSERT_EQ(0, vec->count);
    ASSERT_EQ(4, vec->capacity);
    ASSERT_NE((void*)0, vec->data);
    unordered_vector_destroy(vec);
}

TEST(NAME, pop_empty_vector)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    *(int*)unordered_vector_push_emplace(vec) = 21;
    unordered_vector_pop(vec);
    ASSERT_EQ(NULL, unordered_vector_pop(vec));
    ASSERT_EQ(0, vec->count);
    ASSERT_EQ(2, vec->capacity);
    ASSERT_NE((void*)0, vec->data);
    unordered_vector_destroy(vec);
}

TEST(NAME, pop_clear_freed_vector)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    ASSERT_EQ(NULL, unordered_vector_pop(vec));
    ASSERT_EQ(0, vec->count);
    ASSERT_EQ(0, vec->capacity);
    ASSERT_EQ(NULL, vec->data);
    unordered_vector_destroy(vec);
}

TEST(NAME, get_element_random_access)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    *(int*)unordered_vector_push_emplace(vec) = 53;
    *(int*)unordered_vector_push_emplace(vec) = 24;
    *(int*)unordered_vector_push_emplace(vec) = 73;
    *(int*)unordered_vector_push_emplace(vec) = 43;
    ASSERT_EQ(24, *(int*)unordered_vector_get_element(vec, 1));
    ASSERT_EQ(43, *(int*)unordered_vector_get_element(vec, 3));
    ASSERT_EQ(73, *(int*)unordered_vector_get_element(vec, 2));
    ASSERT_EQ(53, *(int*)unordered_vector_get_element(vec, 0));
    unordered_vector_destroy(vec);
}

TEST(NAME, popping_keeps_existing_elements_in_tact)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    *(int*)unordered_vector_push_emplace(vec) = 53;
    *(int*)unordered_vector_push_emplace(vec) = 24;
    *(int*)unordered_vector_push_emplace(vec) = 73;
    *(int*)unordered_vector_push_emplace(vec) = 43;
    *(int*)unordered_vector_push_emplace(vec) = 24;
    unordered_vector_pop(vec);
    ASSERT_EQ(24, *(int*)unordered_vector_get_element(vec, 1));
    ASSERT_EQ(43, *(int*)unordered_vector_get_element(vec, 3));
    ASSERT_EQ(73, *(int*)unordered_vector_get_element(vec, 2));
    ASSERT_EQ(53, *(int*)unordered_vector_get_element(vec, 0));
    unordered_vector_destroy(vec);
}

TEST(NAME, erasing_by_index_keeps_existing_elements_in_tact)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    *(int*)unordered_vector_push_emplace(vec) = 53;
    *(int*)unordered_vector_push_emplace(vec) = 24;
    *(int*)unordered_vector_push_emplace(vec) = 73;
    *(int*)unordered_vector_push_emplace(vec) = 43;
    *(int*)unordered_vector_push_emplace(vec) = 65;
    unordered_vector_erase_index(vec, 1);
    ASSERT_EQ(53, *(int*)unordered_vector_get_element(vec, 0));
    ASSERT_EQ(65, *(int*)unordered_vector_get_element(vec, 1));
    ASSERT_EQ(73, *(int*)unordered_vector_get_element(vec, 2));
    ASSERT_EQ(43, *(int*)unordered_vector_get_element(vec, 3));
    unordered_vector_erase_index(vec, 1);
    ASSERT_EQ(53, *(int*)unordered_vector_get_element(vec, 0));
    ASSERT_EQ(43, *(int*)unordered_vector_get_element(vec, 1));
    ASSERT_EQ(73, *(int*)unordered_vector_get_element(vec, 2));
    unordered_vector_destroy(vec);
}

TEST(NAME, erasing_by_element_keeps_existing_elements_in_tact)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    *(int*)unordered_vector_push_emplace(vec) = 53;
    *(int*)unordered_vector_push_emplace(vec) = 24;
    *(int*)unordered_vector_push_emplace(vec) = 73;
    *(int*)unordered_vector_push_emplace(vec) = 43;
    *(int*)unordered_vector_push_emplace(vec) = 65;
    unordered_vector_erase_element(vec, unordered_vector_get_element(vec, 1));
    ASSERT_EQ(53, *(int*)unordered_vector_get_element(vec, 0));
    ASSERT_EQ(65, *(int*)unordered_vector_get_element(vec, 1));
    ASSERT_EQ(73, *(int*)unordered_vector_get_element(vec, 2));
    ASSERT_EQ(43, *(int*)unordered_vector_get_element(vec, 3));
    unordered_vector_erase_element(vec, unordered_vector_get_element(vec, 1));
    ASSERT_EQ(53, *(int*)unordered_vector_get_element(vec, 0));
    ASSERT_EQ(43, *(int*)unordered_vector_get_element(vec, 1));
    ASSERT_EQ(73, *(int*)unordered_vector_get_element(vec, 2));
    unordered_vector_destroy(vec);
}

TEST(NAME, get_invalid_index)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    ASSERT_EQ(NULL, unordered_vector_get_element(vec, 1));
    *(int*)unordered_vector_push_emplace(vec) = 53;
    ASSERT_EQ(NULL, unordered_vector_get_element(vec, 1));
    unordered_vector_destroy(vec);
}

TEST(NAME, erase_invalid_index)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    unordered_vector_erase_index(vec, 1);
    unordered_vector_erase_index(vec, 0);
    *(int*)unordered_vector_push_emplace(vec) = 53;
    unordered_vector_erase_index(vec, 1);
    unordered_vector_erase_index(vec, 0);
    unordered_vector_erase_index(vec, 0);
    unordered_vector_destroy(vec);
}
