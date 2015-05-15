#include "gmock/gmock.h"
#include "util/ordered_vector.h"
#include "util/memory.h"

#define NAME ordered_vector_malloc

using namespace testing;

TEST(NAME, create)
{
    force_malloc_fail_on();
    EXPECT_THAT(ordered_vector_create(sizeof(int)), IsNull());
    force_malloc_fail_off();
}

TEST(NAME, push_new_alloc)
{
    struct ordered_vector_t* vec = ordered_vector_create(sizeof(int));
    int a = 6;

    force_malloc_fail_on();
    EXPECT_THAT(ordered_vector_push(vec, &a), Eq(0));
    force_malloc_fail_off();
    EXPECT_THAT(vec->count, Eq(0));
    EXPECT_THAT(vec->capacity, Eq(0));
    EXPECT_THAT(vec->data, IsNull());

    EXPECT_THAT(ordered_vector_push(vec, &a), Ne(0));
    EXPECT_THAT((int*)ordered_vector_back(vec), Pointee(a));
    EXPECT_THAT(vec->count, Eq(1));

    ordered_vector_destroy(vec);
}

TEST(NAME, push_realloc)
{
    struct ordered_vector_t* vec = ordered_vector_create(sizeof(int));
    int a = 4;
    ordered_vector_push(vec, &a);

    while(vec->count < vec->capacity)
        ordered_vector_push(vec, &a);

    void* old_data_ptr = vec->data;
    force_malloc_fail_on();
    EXPECT_THAT(ordered_vector_push(vec, &a), Eq(0));
    force_malloc_fail_off();
    EXPECT_THAT(vec->count, Eq(vec->capacity));
    EXPECT_THAT(vec->data, Eq(old_data_ptr)); /* make sure data is preserved */

    EXPECT_THAT(ordered_vector_push(vec, &a), Ne(0));
    EXPECT_THAT(vec->count, Lt(vec->capacity));
    EXPECT_THAT(vec->data, Ne(old_data_ptr));

    ordered_vector_destroy(vec);
}

TEST(NAME, push_emplace_new_alloc)
{
    struct ordered_vector_t* vec = ordered_vector_create(sizeof(int));
    int a = 6;

    force_malloc_fail_on();
    EXPECT_THAT(ordered_vector_push_emplace(vec), IsNull());
    force_malloc_fail_off();
    EXPECT_THAT(vec->count, Eq(0));
    EXPECT_THAT(vec->capacity, Eq(0));
    EXPECT_THAT(vec->data, IsNull());

    EXPECT_THAT(ordered_vector_push(vec, &a), Ne(0));
    EXPECT_THAT((int*)ordered_vector_back(vec), Pointee(a));
    EXPECT_THAT(vec->count, Eq(1));

    ordered_vector_destroy(vec);
}

TEST(NAME, push_emplace_realloc)
{
    struct ordered_vector_t* vec = ordered_vector_create(sizeof(int));
    int a = 4;
    ordered_vector_push(vec, &a);

    while(vec->count < vec->capacity)
        ordered_vector_push(vec, &a);

    void* old_data_ptr = vec->data;
    force_malloc_fail_on();
    EXPECT_THAT(ordered_vector_push_emplace(vec), IsNull());
    force_malloc_fail_off();
    EXPECT_THAT(vec->count, Eq(vec->capacity));
    EXPECT_THAT(vec->data, Eq(old_data_ptr)); /* make sure data is preserved */

    EXPECT_THAT(ordered_vector_push_emplace(vec), NotNull());
    EXPECT_THAT(vec->count, Lt(vec->capacity));
    EXPECT_THAT(vec->data, Ne(old_data_ptr));

    ordered_vector_destroy(vec);
}

TEST(NAME, push_vector)
{
    struct ordered_vector_t* vec1 = ordered_vector_create(sizeof(int));
    struct ordered_vector_t* vec2 = ordered_vector_create(sizeof(int));
    *(int*)ordered_vector_push_emplace(vec1) = 1;
    *(int*)ordered_vector_push_emplace(vec1) = 2;
    *(int*)ordered_vector_push_emplace(vec2) = 3;
    *(int*)ordered_vector_push_emplace(vec2) = 4;

    force_malloc_fail_on();
    EXPECT_THAT(ordered_vector_push_vector(vec1, vec2), Eq(0));
    force_malloc_fail_off();
    EXPECT_THAT(vec1->count, Eq(2));
    EXPECT_THAT(vec2->count, Eq(2));
    EXPECT_THAT(vec1->capacity, Eq(2));
    EXPECT_THAT(vec2->capacity, Eq(2));

    EXPECT_THAT(ordered_vector_push_vector(vec1, vec2), Ne(0));
    EXPECT_THAT(vec1->count, Eq(4));
    EXPECT_THAT(*(int*)ordered_vector_get_element(vec1, 0), Eq(1));
    EXPECT_THAT(*(int*)ordered_vector_get_element(vec1, 1), Eq(2));
    EXPECT_THAT(*(int*)ordered_vector_get_element(vec1, 2), Eq(3));
    EXPECT_THAT(*(int*)ordered_vector_get_element(vec1, 3), Eq(4));

    ordered_vector_destroy(vec1);
    ordered_vector_destroy(vec2);
}

TEST(NAME, insert_new_alloc)
{
    struct ordered_vector_t* vec = ordered_vector_create(sizeof(int));
    int a = 6;

    force_malloc_fail_on();
    EXPECT_THAT(ordered_vector_insert(vec, 0, &a), Eq(0));
    force_malloc_fail_off();
    EXPECT_THAT(vec->count, Eq(0));
    EXPECT_THAT(vec->capacity, Eq(0));
    EXPECT_THAT(vec->data, IsNull());

    EXPECT_THAT(ordered_vector_insert(vec, 0, &a), Ne(0));
    EXPECT_THAT((int*)ordered_vector_back(vec), Pointee(a));
    EXPECT_THAT(vec->count, Eq(1));

    ordered_vector_destroy(vec);
}

TEST(NAME, insert_realloc)
{
    struct ordered_vector_t* vec = ordered_vector_create(sizeof(int));
    int a = 4;
    ordered_vector_insert(vec, 0, &a);

    while(vec->count < vec->capacity)
        ordered_vector_insert(vec, 0, &a);

    void* old_data_ptr = vec->data;
    force_malloc_fail_on();
    EXPECT_THAT(ordered_vector_insert(vec, 1, &a), Eq(0));
    force_malloc_fail_off();
    EXPECT_THAT(vec->count, Eq(vec->capacity));
    EXPECT_THAT(vec->data, Eq(old_data_ptr)); /* make sure data is preserved */

    EXPECT_THAT(ordered_vector_insert(vec, 1, &a), Ne(0));
    EXPECT_THAT(vec->count, Lt(vec->capacity));
    EXPECT_THAT(vec->data, Ne(old_data_ptr));

    ordered_vector_destroy(vec);
}

TEST(NAME, insert_emplace_new_alloc)
{
    struct ordered_vector_t* vec = ordered_vector_create(sizeof(int));
    int a = 6;

    force_malloc_fail_on();
    EXPECT_THAT(ordered_vector_insert_emplace(vec, 0), IsNull());
    force_malloc_fail_off();
    EXPECT_THAT(vec->count, Eq(0));
    EXPECT_THAT(vec->capacity, Eq(0));
    EXPECT_THAT(vec->data, IsNull());

    EXPECT_THAT(ordered_vector_insert(vec, 0, &a), Ne(0));
    EXPECT_THAT((int*)ordered_vector_back(vec), Pointee(a));
    EXPECT_THAT(vec->count, Eq(1));

    ordered_vector_destroy(vec);
}

TEST(NAME, insert_emplace_realloc)
{
    struct ordered_vector_t* vec = ordered_vector_create(sizeof(int));
    int a = 4;
    ordered_vector_insert(vec, 0, &a);

    while(vec->count < vec->capacity)
        ordered_vector_insert(vec, 0, &a);

    void* old_data_ptr = vec->data;
    force_malloc_fail_on();
    EXPECT_THAT(ordered_vector_insert_emplace(vec, 1), IsNull());
    force_malloc_fail_off();
    EXPECT_THAT(vec->count, Eq(vec->capacity));
    EXPECT_THAT(vec->data, Eq(old_data_ptr)); /* make sure data is preserved */

    EXPECT_THAT(ordered_vector_insert_emplace(vec, 1), NotNull());
    EXPECT_THAT(vec->count, Lt(vec->capacity));
    EXPECT_THAT(vec->data, Ne(old_data_ptr));

    ordered_vector_destroy(vec);
}
