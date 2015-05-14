#include "gmock/gmock.h"
#include "util/ptree.h"
#include "util/memory.h"

#define NAME ptree_malloc

using namespace testing;

TEST(NAME, create)
{
    force_malloc_fail_on();
    ASSERT_THAT(ptree_create(NULL), IsNull());
    force_malloc_fail_off();

    struct ptree_t* tree = ptree_create(NULL);
    ASSERT_THAT(tree, NotNull());
    ptree_destroy(tree);
}

TEST(NAME, add_node_fail_immediately)
{
    struct ptree_t* tree = ptree_create(NULL);

    force_malloc_fail_on();
    ASSERT_THAT(ptree_add_node(tree, "test", NULL), IsNull());
    force_malloc_fail_off();
    ASSERT_THAT(map_count(&tree->children), Eq(0));

    ASSERT_THAT(ptree_add_node(tree, "test", NULL), NotNull());
    ASSERT_THAT(map_count(&tree->children), Eq(1));

    ptree_destroy(tree);
}

TEST(NAME, add_node_fail_in_middle)
{
    struct ptree_t* tree = ptree_create(NULL);

    force_malloc_fail_after(2);
    ASSERT_THAT(ptree_add_node(tree, "test.test.test.test", NULL), IsNull());
    force_malloc_fail_off();
    ASSERT_THAT(map_count(&tree->children), Eq(0));

    force_malloc_fail_after(3);
    ASSERT_THAT(ptree_add_node(tree, "test.test.test.test", NULL), IsNull());
    force_malloc_fail_off();
    ASSERT_THAT(map_count(&tree->children), Eq(0));

    force_malloc_fail_after(4);
    ASSERT_THAT(ptree_add_node(tree, "test.test.test.test", NULL), IsNull());
    force_malloc_fail_off();
    ASSERT_THAT(map_count(&tree->children), Eq(0));

    force_malloc_fail_after(5);
    ASSERT_THAT(ptree_add_node(tree, "test.test.test.test", NULL), IsNull());
    force_malloc_fail_off();
    ASSERT_THAT(map_count(&tree->children), Eq(0));

    force_malloc_fail_after(6);
    ASSERT_THAT(ptree_add_node(tree, "test.test.test.test", NULL), IsNull());
    force_malloc_fail_off();
    ASSERT_THAT(map_count(&tree->children), Eq(0));

    force_malloc_fail_after(7);
    ASSERT_THAT(ptree_add_node(tree, "test.test.test.test", NULL), IsNull());
    force_malloc_fail_off();
    ASSERT_THAT(map_count(&tree->children), Eq(0));

    force_malloc_fail_after(8);
    ASSERT_THAT(ptree_add_node(tree, "test.test.test.test", NULL), IsNull());
    force_malloc_fail_off();
    ASSERT_THAT(map_count(&tree->children), Eq(0));

    ASSERT_THAT(ptree_add_node(tree, "test.test.test.test", NULL), NotNull());
    ASSERT_THAT(map_count(&tree->children), Eq(1));
    ASSERT_THAT(map_count(&ptree_get_node(tree, "test.test.test.test")->children), Eq(0));

    ptree_destroy(tree);
}
