#include "gmock/gmock.h"
#include "util/unordered_vector.h"
#include "util/memory.h"

#define NAME unordered_vector_malloc

using namespace testing;

TEST(NAME, create)
{
    force_malloc_fail_on();
    ASSERT_THAT(unordered_vector_create(sizeof(int)), IsNull());
    force_malloc_fail_off();
}

TEST(NAME, push_new_alloc)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    int a = 6;

    force_malloc_fail_on();
    ASSERT_THAT(unordered_vector_push(vec, &a), Eq(0));
    force_malloc_fail_off();
    ASSERT_THAT(vec->count, Eq(0));
    ASSERT_THAT(vec->capacity, Eq(0));
    ASSERT_THAT(vec->data, IsNull());

    ASSERT_THAT(unordered_vector_push(vec, &a), Ne(0));
    ASSERT_THAT((int*)unordered_vector_back(vec), Pointee(a));
    ASSERT_THAT(vec->count, Eq(1));

    unordered_vector_destroy(vec);
}

TEST(NAME, push_realloc)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    int a = 4;
    unordered_vector_push(vec, &a);

    while(vec->count < vec->capacity)
        unordered_vector_push(vec, &a);

    void* old_data_ptr = vec->data;
    force_malloc_fail_on();
    ASSERT_THAT(unordered_vector_push(vec, &a), Eq(0));
    force_malloc_fail_off();
    ASSERT_THAT(vec->count, Eq(vec->capacity));
    ASSERT_THAT(vec->data, Eq(old_data_ptr)); /* make sure data is preserved */

    ASSERT_THAT(unordered_vector_push(vec, &a), Ne(0));
    ASSERT_THAT(vec->count, Lt(vec->capacity));
    ASSERT_THAT(vec->data, Ne(old_data_ptr));

    unordered_vector_destroy(vec);
}

TEST(NAME, push_emplace_new_alloc)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    int a = 6;

    force_malloc_fail_on();
    ASSERT_THAT(unordered_vector_push_emplace(vec), IsNull());
    force_malloc_fail_off();
    ASSERT_THAT(vec->count, Eq(0));
    ASSERT_THAT(vec->capacity, Eq(0));
    ASSERT_THAT(vec->data, IsNull());

    ASSERT_THAT(unordered_vector_push(vec, &a), Ne(0));
    ASSERT_THAT((int*)unordered_vector_back(vec), Pointee(a));
    ASSERT_THAT(vec->count, Eq(1));

    unordered_vector_destroy(vec);
}

TEST(NAME, push_emplace_realloc)
{
    struct unordered_vector_t* vec = unordered_vector_create(sizeof(int));
    int a = 4;
    unordered_vector_push(vec, &a);

    while(vec->count < vec->capacity)
        unordered_vector_push(vec, &a);

    void* old_data_ptr = vec->data;
    force_malloc_fail_on();
    ASSERT_THAT(unordered_vector_push_emplace(vec), IsNull());
    force_malloc_fail_off();
    ASSERT_THAT(vec->count, Eq(vec->capacity));
    ASSERT_THAT(vec->data, Eq(old_data_ptr)); /* make sure data is preserved */

    ASSERT_THAT(unordered_vector_push_emplace(vec), NotNull());
    ASSERT_THAT(vec->count, Lt(vec->capacity));
    ASSERT_THAT(vec->data, Ne(old_data_ptr));

    unordered_vector_destroy(vec);
}
