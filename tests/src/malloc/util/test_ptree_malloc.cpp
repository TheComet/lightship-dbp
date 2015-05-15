#include "gmock/gmock.h"
#include "util/ptree.h"
#include "util/memory.h"

#define NAME ptree_malloc

using namespace testing;

TEST(NAME, create)
{
    force_malloc_fail_on();
    EXPECT_THAT(ptree_create(NULL), IsNull());
    force_malloc_fail_off();

    struct ptree_t* tree = ptree_create(NULL);
    ASSERT_THAT(tree, NotNull());
    ptree_destroy(tree);
}

TEST(NAME, add_node_fail_immediately)
{
    struct ptree_t* tree = ptree_create(NULL);

    force_malloc_fail_on();
    EXPECT_THAT(ptree_add_node(tree, "test", NULL), IsNull());
    force_malloc_fail_off();
    ASSERT_THAT(map_count(&tree->children), Eq(0));

    ASSERT_THAT(ptree_add_node(tree, "test", NULL), NotNull());
    ASSERT_THAT(map_count(&tree->children), Eq(1));

    ptree_destroy(tree);
}

TEST(NAME, add_node_fail_in_middle)
{
    struct ptree_t* tree = ptree_create(NULL);

    for(int i = 2; i != 9; ++i)
    {
        force_malloc_fail_after(i);
        EXPECT_THAT(ptree_add_node(tree, "test.test.test.test", NULL), IsNull());
        force_malloc_fail_off();
        ASSERT_THAT(map_count(&tree->children), Eq(0));
    }

    ASSERT_THAT(ptree_add_node(tree, "test.test.test.test", NULL), NotNull());
    ASSERT_THAT(map_count(&tree->children), Eq(1));
    ASSERT_THAT(map_count(&ptree_get_node(tree, "test.test.test.test")->children), Eq(0));

    ptree_destroy(tree);
}

TEST(NAME, set_parent_fail_immediately)
{
    struct ptree_t* root = ptree_create(NULL);
    struct ptree_t* node = ptree_create(NULL);

    force_malloc_fail_on();
    EXPECT_THAT(ptree_set_parent(node, root, "node"), Eq(0));
    force_malloc_fail_off();
    ASSERT_THAT(map_count(&root->children), Eq(0));
    ASSERT_THAT(root->parent, IsNull());
    ASSERT_THAT(node->parent, IsNull());

    ASSERT_THAT(ptree_set_parent(node, root, "node"), Ne(0));
    ASSERT_THAT(map_count(&root->children), Eq(1));
    ASSERT_THAT(root->parent, IsNull());
    ASSERT_THAT(node->parent, Eq(root));

    ptree_destroy(root);
}

TEST(NAME, set_parent_fail_in_middle)
{
    struct ptree_t* root = ptree_create(NULL);
    struct ptree_t* node = ptree_create(NULL);

    force_malloc_fail_after(2);
    EXPECT_THAT(ptree_set_parent(node, root, "node"), Eq(0));
    force_malloc_fail_off();
    ASSERT_THAT(map_count(&root->children), Eq(0));
    ASSERT_THAT(root->parent, IsNull());
    ASSERT_THAT(node->parent, IsNull());

    ASSERT_THAT(ptree_set_parent(node, root, "node"), Ne(0));
    ASSERT_THAT(map_count(&root->children), Eq(1));
    ASSERT_THAT(root->parent, IsNull());
    ASSERT_THAT(node->parent, Eq(root));

    ptree_destroy(root);
}

static int* dup_value(int* value)
{
    int* ret = (int*)MALLOC(sizeof *value);
    if(!ret)
        return NULL;
    *ret = *value;
    return ret;
}

static void free_value(int* value)
{
    FREE(value);
}

TEST(NAME, duplicate_tree_fail_immediately)
{
    int *a=(int*)MALLOC(sizeof(int)), *b=(int*)MALLOC(sizeof(int));
    *a = 6, *b = 3;
    struct ptree_t* tree = ptree_create(NULL);
    struct ptree_t* n3 = ptree_add_node(tree, "1.2.3", a);
    struct ptree_t* n1 = ptree_add_node(tree, "1.1.1", b);
    ptree_set_dup_func(n1, (ptree_dup_func)dup_value);
    ptree_set_free_func(n1, (ptree_free_func)free_value);
    ptree_set_dup_func(n3, (ptree_dup_func)dup_value);
    ptree_set_free_func(n3, (ptree_free_func)free_value);

    force_malloc_fail_on();
    EXPECT_THAT(ptree_duplicate_tree(tree), IsNull());
    force_malloc_fail_off();
    ASSERT_THAT(map_count(&tree->children), Eq(1));
    ASSERT_THAT(n3->parent->parent->parent, Eq(tree));
    ASSERT_THAT(n1->parent->parent->parent, Eq(tree));
    ASSERT_THAT(tree->parent, IsNull());
    ASSERT_THAT((int*)ptree_get_node(tree, "1.2.3")->value, Pointee(*a));
    ASSERT_THAT((int*)ptree_get_node(tree, "1.1.1")->value, Pointee(*b));

    struct ptree_t* copy = ptree_duplicate_tree(tree);
    ASSERT_THAT(copy, NotNull());
    ASSERT_THAT(map_count(&copy->children), Eq(1));
    ASSERT_THAT((int*)ptree_get_node(copy, "1.2.3")->value, Pointee(*a));
    ASSERT_THAT((int*)ptree_get_node(copy, "1.1.1")->value, Pointee(*b));
    ASSERT_THAT(copy->parent, IsNull());

    ptree_destroy(tree);
    ptree_destroy(copy);
}

TEST(NAME, duplicate_tree_fail_in_middle)
{
    int *a=(int*)MALLOC(sizeof(int)), *b=(int*)MALLOC(sizeof(int));
    *a = 6, *b = 3;
    struct ptree_t* tree = ptree_create(NULL);
    struct ptree_t* n3 = ptree_add_node(tree, "1.2.3", a);
    struct ptree_t* n1 = ptree_add_node(tree, "1.1.1", b);
    ptree_set_dup_func(n1, (ptree_dup_func)dup_value);
    ptree_set_free_func(n1, (ptree_free_func)free_value);
    ptree_set_dup_func(n3, (ptree_dup_func)dup_value);
    ptree_set_free_func(n3, (ptree_free_func)free_value);

    for(int i = 2; i != 11; ++i)
    {
        force_malloc_fail_after(i);
        EXPECT_THAT(ptree_duplicate_tree(tree), IsNull());
        force_malloc_fail_off();
        ASSERT_THAT(map_count(&tree->children), Eq(1));
        ASSERT_THAT(n3->parent->parent->parent, Eq(tree));
        ASSERT_THAT(n1->parent->parent->parent, Eq(tree));
        ASSERT_THAT(tree->parent, IsNull());
        ASSERT_THAT((int*)ptree_get_node(tree, "1.2.3")->value, Pointee(*a));
        ASSERT_THAT((int*)ptree_get_node(tree, "1.1.1")->value, Pointee(*b));
    }

    struct ptree_t* copy = ptree_duplicate_tree(tree);
    ASSERT_THAT(copy, NotNull());
    ASSERT_THAT(map_count(&copy->children), Eq(1));
    ASSERT_THAT((int*)ptree_get_node(copy, "1.2.3")->value, Pointee(*a));
    ASSERT_THAT((int*)ptree_get_node(copy, "1.1.1")->value, Pointee(*b));
    ASSERT_THAT(copy->parent, IsNull());

    ptree_destroy(tree);
    ptree_destroy(copy);
}

TEST(NAME, duplicate_into_existing_node_fail_immediately)
{
    int *a=(int*)MALLOC(sizeof(int)), *b=(int*)MALLOC(sizeof(int));
    *a = 6, *b = 3;
    struct ptree_t* tree = ptree_create(NULL);
    struct ptree_t* n3 = ptree_add_node(tree, "1.2.3", a);
    struct ptree_t* n2 = ptree_get_node(tree, "1.2");
    struct ptree_t* n1 = ptree_add_node(tree, "1.1.1", b);
    ptree_set_dup_func(n1, (ptree_dup_func)dup_value);
    ptree_set_free_func(n1, (ptree_free_func)free_value);
    ptree_set_dup_func(n3, (ptree_dup_func)dup_value);
    ptree_set_free_func(n3, (ptree_free_func)free_value);

    force_malloc_fail_on();
    EXPECT_THAT(ptree_duplicate_children_into_existing_node(n2, tree), Eq(0));
    force_malloc_fail_off();
    ASSERT_THAT(map_count(&tree->children), Eq(1));
    ASSERT_THAT(map_count(&n2->children), Eq(1));
    ASSERT_THAT(tree->parent, IsNull());
    ASSERT_THAT((int*)ptree_get_node(tree, "1.2.3")->value, Pointee(*a));
    ASSERT_THAT((int*)ptree_get_node(tree, "1.1.1")->value, Pointee(*b));

    ptree_destroy(tree);
}

TEST(NAME, duplicate_into_existing_node_fail_in_middle)
{
    int *a=(int*)MALLOC(sizeof(int)), *b=(int*)MALLOC(sizeof(int));
    *a = 6, *b = 3;
    struct ptree_t* tree = ptree_create(NULL);
    struct ptree_t* n3 = ptree_add_node(tree, "1.2.3", a);
    struct ptree_t* n2 = ptree_get_node(tree, "1.2");
    struct ptree_t* n1 = ptree_add_node(tree, "1.1.1", b);
    ptree_set_dup_func(n1, (ptree_dup_func)dup_value);
    ptree_set_free_func(n1, (ptree_free_func)free_value);
    ptree_set_dup_func(n3, (ptree_dup_func)dup_value);
    ptree_set_free_func(n3, (ptree_free_func)free_value);

    for(int i = 2; i != 18; ++i)
    {
        force_malloc_fail_after(i);
        EXPECT_THAT(ptree_duplicate_children_into_existing_node(n2, tree), Eq(0));
        force_malloc_fail_off();
        ASSERT_THAT(map_count(&tree->children), Eq(1));
        ASSERT_THAT(map_count(&n2->children), Eq(1));
        ASSERT_THAT(tree->parent, IsNull());
        ASSERT_THAT((int*)ptree_get_node(tree, "1.2.3")->value, Pointee(*a));
        ASSERT_THAT((int*)ptree_get_node(tree, "1.1.1")->value, Pointee(*b));
    }

    ptree_destroy(tree);
}

TEST(NAME, get_node)
{
    struct ptree_t* tree = ptree_create(NULL);
    ptree_add_node(tree, "1.1.1", NULL);

    force_malloc_fail_on();
    EXPECT_THAT(ptree_get_node(tree, "1"), IsNull());
    force_malloc_fail_off();

    EXPECT_THAT(ptree_get_node(tree, "1"), NotNull());

    ptree_destroy(tree);
}
