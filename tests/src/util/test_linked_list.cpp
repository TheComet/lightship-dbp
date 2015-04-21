#include "gmock/gmock.h"
#include "util/linked_list.h"

#define NAME linked_list

TEST(NAME, init)
{
    struct list_t list;
    
    list.count = 384;
    list.head = 345;
    list.tail = 232;
    list_init_list(&list, sizeof(int));
    
    ASSERT_EQ(0, list.count);
    ASSERT_EQ(NULL, list.head);
    ASSERT_EQ(NULL, list.tail);
    ASSERT_EQ(sizeof(int), list.element_size);
}

TEST(NAME, create_initialises_listtor)
{
    struct list_t* list = ordered_listtor_create(sizeof(int));
    ASSERT_EQ(0, list->capacity);
    ASSERT_EQ(0, list->count);
    ASSERT_EQ(NULL, list->data);
    ASSERT_EQ(sizeof(int), list->element_size);
    ordered_listtor_destroy(list);
}

TEST(NAME, push_increments_count_and_causes_realloc_by_factor_2)
{
    struct list_t* list = ordered_listtor_create(sizeof(int));
    int x = 9;

    ordered_listtor_push(list, &x);
    ASSERT_EQ(2, list->capacity);
    ASSERT_EQ(1, list->count);

    ordered_listtor_push(list, &x);
    ASSERT_EQ(2, list->capacity);
    ASSERT_EQ(2, list->count);

    ordered_listtor_push(list, &x);
    ASSERT_EQ(4, list->capacity);
    ASSERT_EQ(3, list->count);

    ordered_listtor_destroy(list);
}

TEST(NAME, clear_keeps_buffer_and_resets_count)
{
    struct list_t* list = ordered_listtor_create(sizeof(int));
    int x = 9;
    ordered_listtor_push(list, &x);
    ordered_listtor_clear(list);
    ASSERT_EQ(0, list->count);
    ASSERT_EQ(2, list->capacity);
    ASSERT_NE((void*)0, list->data);
    ordered_listtor_destroy(list);
}

TEST(NAME, clear_free_deletes_buffer_and_resets_count)
{
    struct list_t* list = ordered_listtor_create(sizeof(int));
    int x = 9;
    ordered_listtor_push(list, &x);
    ordered_listtor_clear_free(list);
    ASSERT_EQ(0, list->count);
    ASSERT_EQ(0, list->capacity);
    ASSERT_EQ(NULL, list->data);
    ordered_listtor_destroy(list);
}

TEST(NAME, push_emplace_increments_count_and_causes_realloc_by_factor_2)
{
    struct list_t* list = ordered_listtor_create(sizeof(int));
    ordered_listtor_push_emplace(list);
    ASSERT_EQ(2, list->capacity);
    ASSERT_EQ(1, list->count);
    ordered_listtor_push_emplace(list);
    ASSERT_EQ(2, list->capacity);
    ASSERT_EQ(2, list->count);
    ordered_listtor_push_emplace(list);
    ASSERT_EQ(4, list->capacity);
    ASSERT_EQ(3, list->count);
    ordered_listtor_destroy(list);
}

TEST(NAME, pop_returns_pushed_values)
{
    struct list_t* list = ordered_listtor_create(sizeof(int));
    int x;
    x = 3; ordered_listtor_push(list, &x);
    x = 2; ordered_listtor_push(list, &x);
    x = 6; ordered_listtor_push(list, &x);
    ASSERT_EQ(6, *(int*)ordered_listtor_pop(list));
    x = 23; ordered_listtor_push(list, &x);
    x = 21; ordered_listtor_push(list, &x);
    ASSERT_EQ(21, *(int*)ordered_listtor_pop(list));
    ASSERT_EQ(23, *(int*)ordered_listtor_pop(list));
    ASSERT_EQ(2, *(int*)ordered_listtor_pop(list));
    ASSERT_EQ(3, *(int*)ordered_listtor_pop(list));
    ASSERT_EQ(0, list->count);
    ASSERT_EQ(4, list->capacity);
    ASSERT_NE((void*)0, list->data);
    ordered_listtor_destroy(list);
}

TEST(NAME, pop_returns_push_emplaced_values)
{
    struct list_t* list = ordered_listtor_create(sizeof(int));
    *(int*)ordered_listtor_push_emplace(list) = 53;
    *(int*)ordered_listtor_push_emplace(list) = 24;
    *(int*)ordered_listtor_push_emplace(list) = 73;
    ASSERT_EQ(73, *(int*)ordered_listtor_pop(list));
    *(int*)ordered_listtor_push_emplace(list) = 28;
    *(int*)ordered_listtor_push_emplace(list) = 72;
    ASSERT_EQ(72, *(int*)ordered_listtor_pop(list));
    ASSERT_EQ(28, *(int*)ordered_listtor_pop(list));
    ASSERT_EQ(24, *(int*)ordered_listtor_pop(list));
    ASSERT_EQ(53, *(int*)ordered_listtor_pop(list));
    ASSERT_EQ(0, list->count);
    ASSERT_EQ(4, list->capacity);
    ASSERT_NE((void*)0, list->data);
    ordered_listtor_destroy(list);
}

TEST(NAME, pop_empty_listtor)
{
    struct list_t* list = ordered_listtor_create(sizeof(int));
    *(int*)ordered_listtor_push_emplace(list) = 21;
    ordered_listtor_pop(list);
    ASSERT_EQ(NULL, ordered_listtor_pop(list));
    ASSERT_EQ(0, list->count);
    ASSERT_EQ(2, list->capacity);
    ASSERT_NE((void*)0, list->data);
    ordered_listtor_destroy(list);
}

TEST(NAME, pop_clear_freed_listtor)
{
    struct list_t* list = ordered_listtor_create(sizeof(int));
    ASSERT_EQ(NULL, ordered_listtor_pop(list));
    ASSERT_EQ(0, list->count);
    ASSERT_EQ(0, list->capacity);
    ASSERT_EQ(NULL, list->data);
    ordered_listtor_destroy(list);
}

TEST(NAME, get_element_random_access)
{
    struct list_t* list = ordered_listtor_create(sizeof(int));
    *(int*)ordered_listtor_push_emplace(list) = 53;
    *(int*)ordered_listtor_push_emplace(list) = 24;
    *(int*)ordered_listtor_push_emplace(list) = 73;
    *(int*)ordered_listtor_push_emplace(list) = 43;
    ASSERT_EQ(24, *(int*)ordered_listtor_get_element(list, 1));
    ASSERT_EQ(43, *(int*)ordered_listtor_get_element(list, 3));
    ASSERT_EQ(73, *(int*)ordered_listtor_get_element(list, 2));
    ASSERT_EQ(53, *(int*)ordered_listtor_get_element(list, 0));
    ordered_listtor_destroy(list);
}

TEST(NAME, popping_preserves_existing_elements)
{
    struct list_t* list = ordered_listtor_create(sizeof(int));
    *(int*)ordered_listtor_push_emplace(list) = 53;
    *(int*)ordered_listtor_push_emplace(list) = 24;
    *(int*)ordered_listtor_push_emplace(list) = 73;
    *(int*)ordered_listtor_push_emplace(list) = 43;
    *(int*)ordered_listtor_push_emplace(list) = 24;
    ordered_listtor_pop(list);
    ASSERT_EQ(24, *(int*)ordered_listtor_get_element(list, 1));
    ASSERT_EQ(43, *(int*)ordered_listtor_get_element(list, 3));
    ASSERT_EQ(73, *(int*)ordered_listtor_get_element(list, 2));
    ASSERT_EQ(53, *(int*)ordered_listtor_get_element(list, 0));
    ordered_listtor_destroy(list);
}

TEST(NAME, erasing_by_index_preserves_existing_elements)
{
    struct list_t* list = ordered_listtor_create(sizeof(int));
    *(int*)ordered_listtor_push_emplace(list) = 53;
    *(int*)ordered_listtor_push_emplace(list) = 24;
    *(int*)ordered_listtor_push_emplace(list) = 73;
    *(int*)ordered_listtor_push_emplace(list) = 43;
    *(int*)ordered_listtor_push_emplace(list) = 65;
    ordered_listtor_erase_index(list, 1);
    ASSERT_EQ(53, *(int*)ordered_listtor_get_element(list, 0));
    ASSERT_EQ(73, *(int*)ordered_listtor_get_element(list, 1));
    ASSERT_EQ(43, *(int*)ordered_listtor_get_element(list, 2));
    ASSERT_EQ(65, *(int*)ordered_listtor_get_element(list, 3));
    ordered_listtor_erase_index(list, 1);
    ASSERT_EQ(53, *(int*)ordered_listtor_get_element(list, 0));
    ASSERT_EQ(43, *(int*)ordered_listtor_get_element(list, 1));
    ASSERT_EQ(65, *(int*)ordered_listtor_get_element(list, 2));
    ordered_listtor_destroy(list);
}

TEST(NAME, erasing_by_element_preserves_existing_elements)
{
    struct list_t* list = ordered_listtor_create(sizeof(int));
    *(int*)ordered_listtor_push_emplace(list) = 53;
    *(int*)ordered_listtor_push_emplace(list) = 24;
    *(int*)ordered_listtor_push_emplace(list) = 73;
    *(int*)ordered_listtor_push_emplace(list) = 43;
    *(int*)ordered_listtor_push_emplace(list) = 65;
    ordered_listtor_erase_element(list, ordered_listtor_get_element(list, 1));
    ASSERT_EQ(53, *(int*)ordered_listtor_get_element(list, 0));
    ASSERT_EQ(73, *(int*)ordered_listtor_get_element(list, 1));
    ASSERT_EQ(43, *(int*)ordered_listtor_get_element(list, 2));
    ASSERT_EQ(65, *(int*)ordered_listtor_get_element(list, 3));
    ordered_listtor_erase_element(list, ordered_listtor_get_element(list, 1));
    ASSERT_EQ(53, *(int*)ordered_listtor_get_element(list, 0));
    ASSERT_EQ(43, *(int*)ordered_listtor_get_element(list, 1));
    ASSERT_EQ(65, *(int*)ordered_listtor_get_element(list, 2));
    ordered_listtor_destroy(list);
}

TEST(NAME, get_invalid_index)
{
    struct list_t* list = ordered_listtor_create(sizeof(int));
    ASSERT_EQ(NULL, ordered_listtor_get_element(list, 1));
    *(int*)ordered_listtor_push_emplace(list) = 53;
    ASSERT_EQ(NULL, ordered_listtor_get_element(list, 1));
    ordered_listtor_destroy(list);
}

TEST(NAME, erase_invalid_index)
{
    struct list_t* list = ordered_listtor_create(sizeof(int));
    ordered_listtor_erase_index(list, 1);
    ordered_listtor_erase_index(list, 0);
    *(int*)ordered_listtor_push_emplace(list) = 53;
    ordered_listtor_erase_index(list, 1);
    ordered_listtor_erase_index(list, 0);
    ordered_listtor_erase_index(list, 0);
    ordered_listtor_destroy(list);
}

/* ========================================================================= */
/* EVERYTHING ABOVE THIS POINT IS IDENTICAL TO unordered_listtor              */
/* ========================================================================= */
/* EVERYTHING BELOW THIS POINT IS UNIQUE TO ordered_listtor                   */
/* ========================================================================= */

TEST(NAME, inserting_preserves_existing_elements)
{
    struct list_t* list = ordered_listtor_create(sizeof(int));
    *(int*)ordered_listtor_push_emplace(list) = 53;
    *(int*)ordered_listtor_push_emplace(list) = 24;
    *(int*)ordered_listtor_push_emplace(list) = 73;
    *(int*)ordered_listtor_push_emplace(list) = 43;
    *(int*)ordered_listtor_push_emplace(list) = 65;
    
    int x = 68;
    ordered_listtor_insert(list, 2, &x); // middle insertion
    
    ASSERT_EQ(53, *(int*)ordered_listtor_get_element(list, 0));
    ASSERT_EQ(24, *(int*)ordered_listtor_get_element(list, 1));
    ASSERT_EQ(68, *(int*)ordered_listtor_get_element(list, 2));
    ASSERT_EQ(73, *(int*)ordered_listtor_get_element(list, 3));
    ASSERT_EQ(43, *(int*)ordered_listtor_get_element(list, 4));
    ASSERT_EQ(65, *(int*)ordered_listtor_get_element(list, 5));
    
    x = 16;
    ordered_listtor_insert(list, 0, &x); // beginning insertion
    
    ASSERT_EQ(16, *(int*)ordered_listtor_get_element(list, 0));
    ASSERT_EQ(53, *(int*)ordered_listtor_get_element(list, 1));
    ASSERT_EQ(24, *(int*)ordered_listtor_get_element(list, 2));
    ASSERT_EQ(68, *(int*)ordered_listtor_get_element(list, 3));
    ASSERT_EQ(73, *(int*)ordered_listtor_get_element(list, 4));
    ASSERT_EQ(43, *(int*)ordered_listtor_get_element(list, 5));
    ASSERT_EQ(65, *(int*)ordered_listtor_get_element(list, 6));
    
    x = 82;
    ordered_listtor_insert(list, 7, &x); // end insertion
    
    ASSERT_EQ(16, *(int*)ordered_listtor_get_element(list, 0));
    ASSERT_EQ(53, *(int*)ordered_listtor_get_element(list, 1));
    ASSERT_EQ(24, *(int*)ordered_listtor_get_element(list, 2));
    ASSERT_EQ(68, *(int*)ordered_listtor_get_element(list, 3));
    ASSERT_EQ(73, *(int*)ordered_listtor_get_element(list, 4));
    ASSERT_EQ(43, *(int*)ordered_listtor_get_element(list, 5));
    ASSERT_EQ(65, *(int*)ordered_listtor_get_element(list, 6));
    ASSERT_EQ(82, *(int*)ordered_listtor_get_element(list, 7));
    
    x = 37;
    ordered_listtor_insert(list, 7, &x); // end insertion
    
    ASSERT_EQ(16, *(int*)ordered_listtor_get_element(list, 0));
    ASSERT_EQ(53, *(int*)ordered_listtor_get_element(list, 1));
    ASSERT_EQ(24, *(int*)ordered_listtor_get_element(list, 2));
    ASSERT_EQ(68, *(int*)ordered_listtor_get_element(list, 3));
    ASSERT_EQ(73, *(int*)ordered_listtor_get_element(list, 4));
    ASSERT_EQ(43, *(int*)ordered_listtor_get_element(list, 5));
    ASSERT_EQ(65, *(int*)ordered_listtor_get_element(list, 6));
    ASSERT_EQ(37, *(int*)ordered_listtor_get_element(list, 7));
    ASSERT_EQ(82, *(int*)ordered_listtor_get_element(list, 8));
    
    ordered_listtor_destroy(list);
}

TEST(NAME, insert_emplacing_preserves_existing_elements)
{
    struct list_t* list = ordered_listtor_create(sizeof(int));
    *(int*)ordered_listtor_push_emplace(list) = 53;
    *(int*)ordered_listtor_push_emplace(list) = 24;
    *(int*)ordered_listtor_push_emplace(list) = 73;
    *(int*)ordered_listtor_push_emplace(list) = 43;
    *(int*)ordered_listtor_push_emplace(list) = 65;

    *(int*)ordered_listtor_insert_emplace(list, 2) = 68; // middle insertion
    
    ASSERT_EQ(53, *(int*)ordered_listtor_get_element(list, 0));
    ASSERT_EQ(24, *(int*)ordered_listtor_get_element(list, 1));
    ASSERT_EQ(68, *(int*)ordered_listtor_get_element(list, 2));
    ASSERT_EQ(73, *(int*)ordered_listtor_get_element(list, 3));
    ASSERT_EQ(43, *(int*)ordered_listtor_get_element(list, 4));
    ASSERT_EQ(65, *(int*)ordered_listtor_get_element(list, 5));

    *(int*)ordered_listtor_insert_emplace(list, 0) = 16; // beginning insertion
    
    ASSERT_EQ(16, *(int*)ordered_listtor_get_element(list, 0));
    ASSERT_EQ(53, *(int*)ordered_listtor_get_element(list, 1));
    ASSERT_EQ(24, *(int*)ordered_listtor_get_element(list, 2));
    ASSERT_EQ(68, *(int*)ordered_listtor_get_element(list, 3));
    ASSERT_EQ(73, *(int*)ordered_listtor_get_element(list, 4));
    ASSERT_EQ(43, *(int*)ordered_listtor_get_element(list, 5));
    ASSERT_EQ(65, *(int*)ordered_listtor_get_element(list, 6));

    *(int*)ordered_listtor_insert_emplace(list, 7) = 82; // end insertion
    
    ASSERT_EQ(16, *(int*)ordered_listtor_get_element(list, 0));
    ASSERT_EQ(53, *(int*)ordered_listtor_get_element(list, 1));
    ASSERT_EQ(24, *(int*)ordered_listtor_get_element(list, 2));
    ASSERT_EQ(68, *(int*)ordered_listtor_get_element(list, 3));
    ASSERT_EQ(73, *(int*)ordered_listtor_get_element(list, 4));
    ASSERT_EQ(43, *(int*)ordered_listtor_get_element(list, 5));
    ASSERT_EQ(65, *(int*)ordered_listtor_get_element(list, 6));
    ASSERT_EQ(82, *(int*)ordered_listtor_get_element(list, 7));

    *(int*)ordered_listtor_insert_emplace(list, 7) = 37; // end insertion
    
    ASSERT_EQ(16, *(int*)ordered_listtor_get_element(list, 0));
    ASSERT_EQ(53, *(int*)ordered_listtor_get_element(list, 1));
    ASSERT_EQ(24, *(int*)ordered_listtor_get_element(list, 2));
    ASSERT_EQ(68, *(int*)ordered_listtor_get_element(list, 3));
    ASSERT_EQ(73, *(int*)ordered_listtor_get_element(list, 4));
    ASSERT_EQ(43, *(int*)ordered_listtor_get_element(list, 5));
    ASSERT_EQ(65, *(int*)ordered_listtor_get_element(list, 6));
    ASSERT_EQ(37, *(int*)ordered_listtor_get_element(list, 7));
    ASSERT_EQ(82, *(int*)ordered_listtor_get_element(list, 8));
    
    ordered_listtor_destroy(list);
}
