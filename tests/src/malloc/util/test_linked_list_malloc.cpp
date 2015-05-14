#include "gmock/gmock.h"
#include "util/linked_list.h"
#include "util/memory.h"

#define NAME linked_list_malloc

using namespace testing;

TEST(NAME, list_create)
{
    struct list_t* list = list_create();
    ASSERT_THAT(list, NotNull());
    list_destroy(list);

    force_malloc_fail_on();
    ASSERT_THAT(list_create(), IsNull());
    force_malloc_fail_off();
}

TEST(NAME, list_push)
{
    int a=7, b=3;

    struct list_t* list = list_create();
    list_push(list, &a);
    list_push(list, &b);

    force_malloc_fail_on();
    ASSERT_THAT(list_push(list, NULL), Eq(0));
    force_malloc_fail_off();

    ASSERT_THAT(list->count, Eq(2));
    ASSERT_THAT((int*)list->tail->data, Pointee(a));
    ASSERT_THAT((int*)list->tail->next->data, Pointee(b));

    list_destroy(list);
}

TEST(NAME, list_clear)
{
    int a=7, b=3;

    struct list_t* list = list_create();
    list_push(list, &a);
    list_push(list, &b);

    force_malloc_fail_on();
    list_clear(list);
    force_malloc_fail_off();

    ASSERT_THAT(list->count, Eq(0));
    ASSERT_THAT(list->tail, IsNull());
    ASSERT_THAT(list->head, IsNull());

    list_destroy(list);
}

TEST(NAME, list_pop)
{
    int a=7, b=3;

    struct list_t* list = list_create();
    list_push(list, &a);
    list_push(list, &b);

    force_malloc_fail_on();
    int* val = (int*)list_pop(list);
    force_malloc_fail_off();

    ASSERT_THAT(list->count, Eq(1));
    ASSERT_THAT((int*)list->tail->data, Pointee(a));
    ASSERT_THAT((int*)list->head->data, Pointee(a));
    ASSERT_THAT(val, Pointee(b));

    list_destroy(list);
}

TEST(NAME, list_erase_node)
{
    int a=7, b=3, c=8;

    struct list_t* list = list_create();
    list_push(list, &a);
    list_push(list, &b);
    list_push(list, &c);

    force_malloc_fail_on();
    int* val = (int*)list_erase_node(list, list->tail->next); /* erase middle node */
    force_malloc_fail_off();

    ASSERT_THAT(list->count, Eq(2));
    ASSERT_THAT((int*)list->tail->data, Pointee(a));
    ASSERT_THAT((int*)list->head->data, Pointee(c));
    ASSERT_THAT(val, Pointee(b));

    list_destroy(list);
}

TEST(NAME, list_erase_element)
{
    int a=7, b=3, c=8;

    struct list_t* list = list_create();
    list_push(list, &a);
    list_push(list, &b);
    list_push(list, &c);

    force_malloc_fail_on();
    ASSERT_THAT(list_erase_element(list, &b), Ne(0)); /* erase middle node */
    force_malloc_fail_off();

    ASSERT_THAT(list->count, Eq(2));
    ASSERT_THAT((int*)list->tail->data, Pointee(a));
    ASSERT_THAT((int*)list->head->data, Pointee(c));
    
    list_destroy(list);
}
