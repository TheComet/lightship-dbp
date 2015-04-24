#include "gmock/gmock.h"
#include "util/ptree.h"
#include "util/memory.h"

#define NAME ptree

using testing::IsNull;

TEST(NAME, init)
{
    /* init with some garbage values */
    struct ptree_t tree;
    int a = 6;
    tree.children.vector.element_size = 2935;
    tree.children.vector.capacity = 282;
    tree.children.vector.count = 2358;
    tree.children.vector.data = (DATA_POINTER_TYPE*)239842;
    tree.dup_value = (ptree_dup_func)20398;
    tree.free_value = (ptree_free_func)230027;
    tree.value = (void*)928377;

    ptree_init_ptree(&tree, &a);

    ASSERT_EQ(sizeof(struct map_key_value_t), tree.children.vector.element_size);
    ASSERT_EQ(0, tree.children.vector.capacity);
    ASSERT_EQ(0, tree.children.vector.count);
    EXPECT_THAT(tree.children.vector.data, IsNull());
    EXPECT_THAT(tree.dup_value, IsNull());
    EXPECT_THAT(tree.free_value, IsNull());
    ASSERT_EQ(&a, tree.value);

    ptree_destroy_keep_root(&tree, 0);
}

TEST(NAME, destroy_and_keep_root)
{
    struct ptree_t* tree = (struct ptree_t*)MALLOC(sizeof *tree);
    ptree_init_ptree(tree, NULL);
    ptree_destroy_keep_root(tree, 0);
    FREE(tree);
}

TEST(NAME, destroy_and_free_root)
{
    struct ptree_t* tree = ptree_create(NULL);
    ptree_destroy(tree, 0);
}

TEST(NAME, destroy_and_free_values)
{
    int* a = (int*)MALLOC(sizeof(int)); *a = 4;
    int* b = (int*)MALLOC(sizeof(int)); *b = 7;
    int* c = (int*)MALLOC(sizeof(int)); *c = 3;
    int* d = (int*)MALLOC(sizeof(int)); *d = 6;
    struct ptree_t* tree = ptree_create(NULL);

    struct ptree_t* node;
    node = ptree_add_node(tree, "node1", a); ptree_set_free_func(node, FREE);
    node = ptree_add_node(tree, "node2", b); ptree_set_free_func(node, FREE);
    node = ptree_add_node(tree, "node3", c); ptree_set_free_func(node, FREE);
    node = ptree_add_node(tree, "node4", d); ptree_set_free_func(node, FREE);

    ptree_destroy(tree, 1);
}
