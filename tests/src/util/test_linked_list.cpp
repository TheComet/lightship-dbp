#include "gmock/gmock.h"
#include "util/linked_list.h"

#define NAME linked_list

using namespace testing;

TEST(NAME, init)
{
    struct list_t list;

    /* init to garbage values */
    list.count = 384;
    list.head = (struct list_node_t*)345;
    list.tail = (struct list_node_t*)232;
    list_init_list(&list);

    EXPECT_THAT(list.count, Eq(0));
    EXPECT_THAT(list.head, IsNull());
    EXPECT_THAT(list.tail, IsNull());
}

TEST(NAME, create_initialises_list)
{
    struct list_t* list = list_create();
    EXPECT_THAT(list->count, Eq(0));
    EXPECT_THAT(list->head, IsNull());
    EXPECT_THAT(list->tail, IsNull());
    list_destroy(list);
}

TEST(NAME, push_correctly_links_nodes)
{
    struct list_t* list = list_create();
    int a = 7;
    int b = 4;
    int c = 2;

    list_push(list, &a);
    // make sure links are in tact
    ASSERT_EQ(list->head, list->tail);
    EXPECT_THAT(list->head, NotNull());
    EXPECT_THAT(list->tail->prev, IsNull());
    EXPECT_THAT(list->head->next, IsNull());
    // make sure counter is keeping track
    ASSERT_EQ(1, list->count);

    list_push(list, &b);
    // make sure links are in tact
    ASSERT_EQ(list->head, list->tail->next);
    ASSERT_EQ(list->tail, list->head->prev);
    EXPECT_THAT(list->tail->prev, IsNull());
    EXPECT_THAT(list->head->next, IsNull());
    // make sure counter is keeping track
    ASSERT_EQ(2, list->count);

    list_push(list, &c);
    // make sure links are in tact
    ASSERT_EQ(list->head, list->tail->next->next);
    ASSERT_EQ(list->head->prev, list->tail->next);
    ASSERT_EQ(list->head->prev->prev, list->tail);
    EXPECT_THAT(list->tail->prev, IsNull());
    EXPECT_THAT(list->head->next, IsNull());
    // make sure counter is keeping track
    ASSERT_EQ(3, list->count);

    // make sure data is correctly being referenced
    ASSERT_EQ(a, *(int*)list->tail->data);
    ASSERT_EQ(b, *(int*)list->tail->next->data);
    ASSERT_EQ(c, *(int*)list->tail->next->next->data);

    list_destroy(list);
}

TEST(NAME, clear_deletes_all_nodes)
{
    struct list_t* list = list_create();

    list_push(list, NULL);
    list_push(list, NULL);
    list_push(list, NULL);
    list_push(list, NULL);

    list_clear(list);
    EXPECT_THAT(list->tail, IsNull());
    EXPECT_THAT(list->head, IsNull());
    ASSERT_EQ(0, list->count);

    list_destroy(list);
}

TEST(NAME, pop_correctly_links_nodes)
{
    struct list_t* list = list_create();

    int a = 2;
    int b = 8;
    int c = 4;
    int d = 9;

    list_push(list, &a);
    list_push(list, &b);
    list_push(list, &c);
    list_push(list, &d);

    ASSERT_EQ(d, *(int*)list_pop(list));
    ASSERT_EQ(list->tail, list->head->prev->prev);
    ASSERT_EQ(list->tail->next, list->head->prev);
    ASSERT_EQ(list->tail->next->next, list->head);
    EXPECT_THAT(list->tail->prev, IsNull());
    EXPECT_THAT(list->head->next, IsNull());
    ASSERT_EQ(3, list->count);

    ASSERT_EQ(c, *(int*)list_pop(list));
    ASSERT_EQ(list->tail, list->head->prev);
    ASSERT_EQ(list->tail->next, list->head);
    EXPECT_THAT(list->head->next, IsNull());
    EXPECT_THAT(list->tail->prev, IsNull());
    ASSERT_EQ(2, list->count);

    ASSERT_EQ(b, *(int*)list_pop(list));
    ASSERT_EQ(list->tail, list->head);
    EXPECT_THAT(list->head->next, IsNull());
    EXPECT_THAT(list->tail->prev, IsNull());
    ASSERT_EQ(1, list->count);

    ASSERT_EQ(a, *(int*)list_pop(list));
    EXPECT_THAT(list->tail, IsNull());
    EXPECT_THAT(list->head, IsNull());
    ASSERT_EQ(0, list->count);

    list_destroy(list);
}

TEST(NAME, pop_with_no_elements_returns_null)
{
    struct list_t* list = list_create();

    EXPECT_THAT(list_pop(list), IsNull());
    EXPECT_THAT(list->head, IsNull());
    EXPECT_THAT(list->tail, IsNull());
    ASSERT_EQ(0, list->count);

    list_destroy(list);
}

TEST(NAME, erase_node_from_middle_correctly_links_nodes)
{
    struct list_t* list = list_create();

    int a = 4;
    int b = 8;
    int c = 6;
    int d = 2;

    list_push(list, &a);
    list_push(list, &b);
    list_push(list, &c);
    list_push(list, &d);

    ASSERT_EQ(b, *(int*)list_erase_node(list, list->tail->next));
    ASSERT_EQ(list->head, list->tail->next->next);
    ASSERT_EQ(list->head->prev, list->tail->next);
    ASSERT_EQ(list->head->prev->prev, list->tail);
    EXPECT_THAT(list->head->next, IsNull());
    EXPECT_THAT(list->tail->prev, IsNull());
    ASSERT_EQ(a, *(int*)list->tail->data);
    ASSERT_EQ(c, *(int*)list->tail->next->data);
    ASSERT_EQ(d, *(int*)list->tail->next->next->data);
    ASSERT_EQ(3, list->count);

    list_destroy(list);
}

TEST(NAME, erase_node_from_tail_correctly_links_nodes)
{
    struct list_t* list = list_create();

    int a = 4;
    int b = 8;
    int c = 6;
    int d = 2;

    list_push(list, &a);
    list_push(list, &b);
    list_push(list, &c);
    list_push(list, &d);

    ASSERT_EQ(a, *(int*)list_erase_node(list, list->tail));
    ASSERT_EQ(list->head, list->tail->next->next);
    ASSERT_EQ(list->head->prev, list->tail->next);
    ASSERT_EQ(list->head->prev->prev, list->tail);
    EXPECT_THAT(list->head->next, IsNull());
    EXPECT_THAT(list->tail->prev, IsNull());
    ASSERT_EQ(b, *(int*)list->tail->data);
    ASSERT_EQ(c, *(int*)list->tail->next->data);
    ASSERT_EQ(d, *(int*)list->tail->next->next->data);
    ASSERT_EQ(3, list->count);

    list_destroy(list);
}


TEST(NAME, erase_node_from_head_correctly_links_nodes)
{
    struct list_t* list = list_create();

    int a = 4;
    int b = 8;
    int c = 6;
    int d = 2;

    list_push(list, &a);
    list_push(list, &b);
    list_push(list, &c);
    list_push(list, &d);

    ASSERT_EQ(d, *(int*)list_erase_node(list, list->head));
    ASSERT_EQ(list->head, list->tail->next->next);
    ASSERT_EQ(list->head->prev, list->tail->next);
    ASSERT_EQ(list->head->prev->prev, list->tail);
    EXPECT_THAT(list->head->next, IsNull());
    EXPECT_THAT(list->tail->prev, IsNull());
    ASSERT_EQ(a, *(int*)list->tail->data);
    ASSERT_EQ(b, *(int*)list->tail->next->data);
    ASSERT_EQ(c, *(int*)list->tail->next->next->data);
    ASSERT_EQ(3, list->count);

    list_destroy(list);
}


TEST(NAME, erase_element_from_middle_correctly_links_nodes)
{
    struct list_t* list = list_create();

    int a = 4;
    int b = 8;
    int c = 6;
    int d = 2;

    list_push(list, &a);
    list_push(list, &b);
    list_push(list, &c);
    list_push(list, &d);

    ASSERT_NE(0, list_erase_element(list, &b));
    ASSERT_EQ(list->head, list->tail->next->next);
    ASSERT_EQ(list->head->prev, list->tail->next);
    ASSERT_EQ(list->head->prev->prev, list->tail);
    EXPECT_THAT(list->head->next, IsNull());
    EXPECT_THAT(list->tail->prev, IsNull());
    ASSERT_EQ(a, *(int*)list->tail->data);
    ASSERT_EQ(c, *(int*)list->tail->next->data);
    ASSERT_EQ(d, *(int*)list->tail->next->next->data);
    ASSERT_EQ(3, list->count);

    list_destroy(list);
}

TEST(NAME, erase_element_from_tail_correctly_links_nodes)
{
    struct list_t* list = list_create();

    int a = 4;
    int b = 8;
    int c = 6;
    int d = 2;

    list_push(list, &a);
    list_push(list, &b);
    list_push(list, &c);
    list_push(list, &d);

    ASSERT_NE(0, list_erase_element(list, &a));
    ASSERT_EQ(list->head, list->tail->next->next);
    ASSERT_EQ(list->head->prev, list->tail->next);
    ASSERT_EQ(list->head->prev->prev, list->tail);
    EXPECT_THAT(list->head->next, IsNull());
    EXPECT_THAT(list->tail->prev, IsNull());
    ASSERT_EQ(b, *(int*)list->tail->data);
    ASSERT_EQ(c, *(int*)list->tail->next->data);
    ASSERT_EQ(d, *(int*)list->tail->next->next->data);
    ASSERT_EQ(3, list->count);

    list_destroy(list);
}


TEST(NAME, erase_element_from_head_correctly_links_nodes)
{
    struct list_t* list = list_create();

    int a = 4;
    int b = 8;
    int c = 6;
    int d = 2;

    list_push(list, &a);
    list_push(list, &b);
    list_push(list, &c);
    list_push(list, &d);

    ASSERT_NE(0, list_erase_element(list, &d));
    ASSERT_EQ(list->head, list->tail->next->next);
    ASSERT_EQ(list->head->prev, list->tail->next);
    ASSERT_EQ(list->head->prev->prev, list->tail);
    EXPECT_THAT(list->head->next, IsNull());
    EXPECT_THAT(list->tail->prev, IsNull());
    ASSERT_EQ(a, *(int*)list->tail->data);
    ASSERT_EQ(b, *(int*)list->tail->next->data);
    ASSERT_EQ(c, *(int*)list->tail->next->next->data);
    ASSERT_EQ(3, list->count);

    list_destroy(list);
}

TEST(NAME, erase_non_existing_element_returns_false)
{
    struct list_t* list = list_create();

    int a = 4;
    int b = 8;
    int c = 6;
    int d = 2;

    list_push(list, &a);
    list_push(list, &b);
    list_push(list, &c);

    ASSERT_EQ(0, list_erase_element(list, &d));
    ASSERT_EQ(list->head, list->tail->next->next);
    ASSERT_EQ(list->head->prev, list->tail->next);
    ASSERT_EQ(list->head->prev->prev, list->tail);
    EXPECT_THAT(list->head->next, IsNull());
    EXPECT_THAT(list->tail->prev, IsNull());
    ASSERT_EQ(a, *(int*)list->tail->data);
    ASSERT_EQ(b, *(int*)list->tail->next->data);
    ASSERT_EQ(c, *(int*)list->tail->next->next->data);
    ASSERT_EQ(3, list->count);

    list_destroy(list);
}

TEST(NAME, erase_element_from_empty_list_returns_false)
{
    struct list_t* list = list_create();

    int a = 4;

    ASSERT_EQ(0, list_erase_element(list, &a));
    EXPECT_THAT(list->head, IsNull());
    EXPECT_THAT(list->tail, IsNull());
    ASSERT_EQ(0, list->count);

    list_destroy(list);
}

TEST(NAME, erase_null_element_from_empty_list_returns_false)
{
    struct list_t* list = list_create();

    ASSERT_EQ(0, list_erase_element(list, NULL));
    EXPECT_THAT(list->head, IsNull());
    EXPECT_THAT(list->tail, IsNull());
    ASSERT_EQ(0, list->count);

    list_destroy(list);
}

TEST(NAME, iterate_forwards)
{
    struct list_t* list = list_create();

    int values[] = {3, 7, 4};
    int i = 0;

    list_push(list, &values[0]);
    list_push(list, &values[1]);
    list_push(list, &values[2]);

    { LIST_FOR_EACH(list, int, value)
    {
        ASSERT_EQ(values[i], *value);
        i++;
    }}

    list_destroy(list);
}

TEST(NAME, iterate_backwards)
{
    struct list_t* list = list_create();

    int values[] = {3, 7, 4};
    int i = 2;

    list_push(list, &values[0]);
    list_push(list, &values[1]);
    list_push(list, &values[2]);

    { LIST_FOR_EACH_R(list, int, value)
    {
        ASSERT_EQ(values[i], *value);
        i--;
    }}

    list_destroy(list);
}

TEST(NAME, iterate_forwards_and_erase)
{
    struct list_t* list = list_create();

    int values[] = {3, 7, 4};
    int i = 0;

    list_push(list, &values[0]);
    list_push(list, &values[1]);
    list_push(list, &values[2]);

    { LIST_FOR_EACH_ERASE(list, int, value)
    {
        ASSERT_EQ(values[i], *(int*)list_erase_node(list, node_value));
        i++;
    }}

    EXPECT_THAT(list->head, IsNull());
    EXPECT_THAT(list->tail, IsNull());
    ASSERT_EQ(0, list->count);

    list_destroy(list);
}


TEST(NAME, iterate_backwards_and_erase)
{
    struct list_t* list = list_create();

    int values[] = {3, 7, 4};
    int i = 2;

    list_push(list, &values[0]);
    list_push(list, &values[1]);
    list_push(list, &values[2]);

    { LIST_FOR_EACH_ERASE_R(list, int, value)
    {
        ASSERT_EQ(values[i], *(int*)list_erase_node(list, node_value));
        i--;
    }}

    EXPECT_THAT(list->head, IsNull());
    EXPECT_THAT(list->tail, IsNull());
    ASSERT_EQ(0, list->count);

    list_destroy(list);
}
