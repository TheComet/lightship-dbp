#include "gmock/gmock.h"
#include "util/ptree.h"
#include "util/memory.h"

#define NAME ptree

TEST(NAME, init)
{
    struct ptree_t tree;
    int a = 6;
    tree.children.vector.element_size = 2935;
    tree.children.vector.capacity = 282;
    tree.children.vector.count = 2358;
    tree.children.vector.data = (void*)239842;
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
}

TEST(NAME, destroy_keep_root)
{
    struct ptree_t* tree = (struct ptree_t*)MALLOC(sizeof *tree);
    ptree_init_ptree(tree, NULL);
    ptree_destroy(tree, 0, 0);
    FREE(tree);
}

TEST(NAME, destroy_and_free_root)
{
    struct ptree_t* tree = ptree_create(NULL);
    ptree_destroy(tree, 0, 1);
}

TEST(NAME, destroy_and_free_values)
{
    int* a = (int*)MALLOC(sizeof(int)); *a = 4;
    int* b = (int*)MALLOC(sizeof(int)); *b = 4;
    int* c = (int*)MALLOC(sizeof(int)); *c = 4;
    int* d = (int*)MALLOC(sizeof(int)); *d = 4;
    struct ptree_t* tree = ptree_create(a);
    
}