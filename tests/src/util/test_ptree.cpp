#include "gmock/gmock.h"
#include "util/ptree.h"
#include "util/memory.h"

#define NAME ptree

using namespace testing;

struct MockTreeItem
{
    MOCK_METHOD0(fakeDie, void());
};

void mock_fake_delete_tree_item(MockTreeItem* p)
{
    p->fakeDie();
}

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

    EXPECT_THAT(tree.children.vector.element_size, Eq(sizeof(struct map_key_value_t)));
    EXPECT_THAT(tree.children.vector.capacity, Eq(0));
    EXPECT_THAT(tree.children.vector.count, Eq(0));
    EXPECT_THAT(tree.children.vector.data, IsNull());
    EXPECT_THAT(tree.dup_value, IsNull());
    EXPECT_THAT(tree.free_value, IsNull());
    EXPECT_THAT((int*)tree.value, Pointee(a));

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

TEST(NAME, simple_tree_has_correct_structure)
{
    int a = 3, b = 2, c = 7, d = 4, e = 12, f = 4;

    // root         (a)
    // |_node1      (b)
    // | |_node2    (null)
    // | |_node3    (c)
    // | | |_node4  (d)
    // | |_node5    (e)
    // |_node6      (null)
    // |_node7      (f)
    struct ptree_t* tree  = ptree_create(&a);
    struct ptree_t* node1 = ptree_add_node(tree,  "node1", &b);
    struct ptree_t* node2 = ptree_add_node(node1, "node2", NULL);
    struct ptree_t* node3 = ptree_add_node(node1, "node3", &c);
    struct ptree_t* node4 = ptree_add_node(node3, "node4", &d);
    struct ptree_t* node5 = ptree_add_node(node1, "node5", &e);
    struct ptree_t* node6 = ptree_add_node(tree,  "node6", NULL);
    struct ptree_t* node7 = ptree_add_node(tree,  "node7", &f);

    uint32_t root_hash  = PTREE_HASH_STRING("root");
    uint32_t node1_hash = PTREE_HASH_STRING("node1");
    uint32_t node2_hash = PTREE_HASH_STRING("node2");
    uint32_t node3_hash = PTREE_HASH_STRING("node3");
    uint32_t node4_hash = PTREE_HASH_STRING("node4");
    uint32_t node5_hash = PTREE_HASH_STRING("node5");
    uint32_t node6_hash = PTREE_HASH_STRING("node6");
    uint32_t node7_hash = PTREE_HASH_STRING("node7");

    // check container sizes
    EXPECT_THAT(map_count(&tree->children),  Eq(3));
    EXPECT_THAT(map_count(&node1->children), Eq(3));
    EXPECT_THAT(map_count(&node2->children), Eq(0));
    EXPECT_THAT(map_count(&node3->children), Eq(1));
    EXPECT_THAT(map_count(&node4->children), Eq(0));
    EXPECT_THAT(map_count(&node5->children), Eq(0));
    EXPECT_THAT(map_count(&node6->children), Eq(0));
    EXPECT_THAT(map_count(&node7->children), Eq(0));

    // check structure
    EXPECT_THAT((struct ptree_t*)map_find(&tree->children,  node1_hash), Eq(node1));
    EXPECT_THAT((struct ptree_t*)map_find(&node1->children, node2_hash), Eq(node2));
    EXPECT_THAT((struct ptree_t*)map_find(&node1->children, node3_hash), Eq(node3));
    EXPECT_THAT((struct ptree_t*)map_find(&node3->children, node4_hash), Eq(node4));
    EXPECT_THAT((struct ptree_t*)map_find(&node1->children, node5_hash), Eq(node5));
    EXPECT_THAT((struct ptree_t*)map_find(&tree->children,  node6_hash), Eq(node6));
    EXPECT_THAT((struct ptree_t*)map_find(&tree->children,  node7_hash), Eq(node7));

    // check values
    EXPECT_THAT((int*)tree->value, Pointee(a));
    EXPECT_THAT((int*)node1->value, Pointee(b));
    EXPECT_THAT((int*)node2->value, IsNull());
    EXPECT_THAT((int*)node3->value, Pointee(c));
    EXPECT_THAT((int*)node4->value, Pointee(d));
    EXPECT_THAT((int*)node5->value, Pointee(e));
    EXPECT_THAT((int*)node6->value, IsNull());
    EXPECT_THAT((int*)node7->value, Pointee(f));

    ptree_destroy(tree, 0);
}

TEST(NAME, recursively_destroy_tree_and_free_values)
{
    MockTreeItem a;
    MockTreeItem b;
    MockTreeItem c;
    MockTreeItem d;
    MockTreeItem e;
    MockTreeItem f;

    EXPECT_CALL(a, fakeDie()).Times(1);
    EXPECT_CALL(b, fakeDie()).Times(1);
    EXPECT_CALL(c, fakeDie()).Times(1);
    EXPECT_CALL(d, fakeDie()).Times(1);
    EXPECT_CALL(e, fakeDie()).Times(1);
    EXPECT_CALL(f, fakeDie()).Times(1);

    // root         (a)
    // |_node1      (b)
    // | |_node2    (null)
    // | |_node3    (c)
    // | | |_node4  (d)
    // | |_node5    (e)
    // |_node6      (null)
    // |_node7      (f)
    struct ptree_t* tree  = ptree_create(&a);
    struct ptree_t* node1 = ptree_add_node(tree,  "node1", &b);
    struct ptree_t* node2 = ptree_add_node(node1, "node2", NULL);
    struct ptree_t* node3 = ptree_add_node(node1, "node3", &c);
    struct ptree_t* node4 = ptree_add_node(node3, "node4", &d);
    struct ptree_t* node5 = ptree_add_node(node1, "node5", &e);
    struct ptree_t* node6 = ptree_add_node(tree,  "node6", NULL);
    struct ptree_t* node7 = ptree_add_node(tree,  "node7", &f);

    ptree_set_free_func(tree,  (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node1, (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node2, (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node3, (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node4, (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node5, (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node6, (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node7, (ptree_free_func)mock_fake_delete_tree_item);

    ptree_destroy(tree, 1);
}

TEST(NAME, recursively_destroy_tree_and_free_values_with_missing_free_functions)
{
    MockTreeItem a;
    MockTreeItem b;
    MockTreeItem c;
    MockTreeItem d;
    MockTreeItem e;
    MockTreeItem f;

    EXPECT_CALL(a, fakeDie()).Times(1);
    EXPECT_CALL(b, fakeDie()).Times(1);
    EXPECT_CALL(c, fakeDie()).Times(1);
    EXPECT_CALL(d, fakeDie()).Times(1);
    /* Note - Expecting this to never be called, as its free function will be missing */
    EXPECT_CALL(e, fakeDie()).Times(0);
    EXPECT_CALL(f, fakeDie()).Times(1);

    // root         (a)
    // |_node1      (b)
    // | |_node2    (null)
    // | |_node3    (c)
    // | | |_node4  (d)
    // | |_node5    (e)
    // |_node6      (null)
    // |_node7      (f)
    struct ptree_t* tree  = ptree_create(&a);
    struct ptree_t* node1 = ptree_add_node(tree,  "node1", &b);
    struct ptree_t* node2 = ptree_add_node(node1, "node2", NULL);
    struct ptree_t* node3 = ptree_add_node(node1, "node3", &c);
    struct ptree_t* node4 = ptree_add_node(node3, "node4", &d);
    struct ptree_t* node5 = ptree_add_node(node1, "node5", &e);
    struct ptree_t* node6 = ptree_add_node(tree,  "node6", NULL);
    struct ptree_t* node7 = ptree_add_node(tree,  "node7", &f);

    ptree_set_free_func(tree,  (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node1, (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node2, (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node3, (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node4, (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node5, NULL); /* missing free function */
    ptree_set_free_func(node6, (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node7, (ptree_free_func)mock_fake_delete_tree_item);

    ptree_destroy(tree, 1);
}
