#include "gmock/gmock.h"
#include "util/ptree.h"
#include "util/memory.h"

#define NAME ptree

using namespace testing;

struct MockTreeItem
{
    MOCK_METHOD0(notifyDie, void());
    MOCK_METHOD0(notifyCopy, void());
};

void mock_fake_delete_tree_item(NiceMock<MockTreeItem>* p)
{
    p->notifyDie();
}

void mock_free_tree_item(NiceMock<MockTreeItem>* p)
{
    delete p;
}

void mock_free_tree_item_and_notify(NiceMock<MockTreeItem>* p)
{
    p->notifyDie();
    delete p;
}

NiceMock<MockTreeItem>* mock_dup_tree_item(NiceMock<MockTreeItem>* item)
{
    item->notifyCopy();
    NiceMock<MockTreeItem>* i = new NiceMock<MockTreeItem>;
    return i;
}

NiceMock<MockTreeItem>* mock_dup_tree_item_and_expect_die(NiceMock<MockTreeItem>* item)
{
    item->notifyCopy();
    NiceMock<MockTreeItem>* i = new NiceMock<MockTreeItem>;
    EXPECT_CALL(*i, notifyDie()).Times(1);
    return i;
}

TEST(NAME, init)
{
    // This checks to make sure ptree_init_ptree() initialises the ptree_t
    // structure correctly.

    // init with some garbage values
    struct ptree_t tree;
    int a = 6;
    tree.children.vector.element_size = 2935;
    tree.children.vector.capacity = 282;
    tree.children.vector.count = 2358;
    tree.children.vector.data = (DATA_POINTER_TYPE*)239842;
    tree.parent = (struct ptree_t*)8384;
    tree.dup_value = (ptree_dup_func)20398;
    tree.free_value = (ptree_free_func)230027;
    tree.value = (void*)928377;

    ptree_init_ptree(&tree, &a);

    EXPECT_THAT(tree.children.vector.element_size, Eq(sizeof(struct map_key_value_t)));
    EXPECT_THAT(tree.children.vector.capacity, Eq(0));
    EXPECT_THAT(tree.children.vector.count, Eq(0));
    EXPECT_THAT(tree.children.vector.data, IsNull());
    EXPECT_THAT(tree.parent, IsNull());
    EXPECT_THAT(tree.dup_value, IsNull());
    EXPECT_THAT(tree.free_value, IsNull());
    EXPECT_THAT((int*)tree.value, Pointee(a));

    ptree_destroy_keep_root(&tree, 0);
}

TEST(NAME, destroy_and_keep_root)
{
    // There's currently no way to check if an object was deallocated or not,
    // but the memory reporter will show objects that were double-freed or not
    // deallocated at all.

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

TEST(NAME, add_node_no_fill_in)
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

    // check parents
    EXPECT_THAT(tree->parent, IsNull());
    EXPECT_THAT(node1->parent, Eq(tree));
    EXPECT_THAT(node2->parent, Eq(node1));
    EXPECT_THAT(node3->parent, Eq(node1));
    EXPECT_THAT(node4->parent, Eq(node3));
    EXPECT_THAT(node5->parent, Eq(node1));
    EXPECT_THAT(node6->parent, Eq(tree));
    EXPECT_THAT(node7->parent, Eq(tree));

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

TEST(NAME, add_node_fill_in_missing_middle_nodes)
{
    int a = 3, b = 2, c = 7, d = 4, e = 12, f = 4;

    // root         (a)
    // |_node1      (b)
    // | |_node2    (c)
    // | |_node3    (null)
    // | | |_node4  (d)
    // | |_node5    (e)
    // |_node6      (null)
    // |_node7      (f)
    struct ptree_t* tree  = ptree_create(&a);
    struct ptree_t* node4 = ptree_add_node(tree, "node1.node3.node4", &d);
    struct ptree_t* node2 = ptree_add_node(tree, "node1.node2", &c);
    struct ptree_t* node5 = ptree_add_node(tree, "node1.node5", &e);
    struct ptree_t* node6 = ptree_add_node(tree, "node6", NULL);
    struct ptree_t* node7 = ptree_add_node(tree, "node7", &f);
    
    /* set values of filled in middle nodes */
    struct ptree_t* node1 = ptree_get_node(tree, "node1");
    struct ptree_t* node3 = ptree_get_node(tree, "node1.node3");
    node1->value = &b;

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

    // check parents
    EXPECT_THAT(tree->parent, IsNull());
    EXPECT_THAT(node1->parent, Eq(tree));
    EXPECT_THAT(node2->parent, Eq(node1));
    EXPECT_THAT(node3->parent, Eq(node1));
    EXPECT_THAT(node4->parent, Eq(node3));
    EXPECT_THAT(node5->parent, Eq(node1));
    EXPECT_THAT(node6->parent, Eq(tree));
    EXPECT_THAT(node7->parent, Eq(tree));

    // check values
    EXPECT_THAT((int*)tree->value, Pointee(a));
    EXPECT_THAT((int*)node1->value, Pointee(b));
    EXPECT_THAT((int*)node2->value, Pointee(c));
    EXPECT_THAT((int*)node3->value, IsNull());
    EXPECT_THAT((int*)node4->value, Pointee(d));
    EXPECT_THAT((int*)node5->value, Pointee(e));
    EXPECT_THAT((int*)node6->value, IsNull());
    EXPECT_THAT((int*)node7->value, Pointee(f));

    ptree_destroy(tree, 0);
}

TEST(NAME, clean_tree)
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
    
    /* should return that 2 nodes were cleaned */
    ASSERT_THAT(ptree_clean(tree), Eq(2));
    
    
}

TEST(NAME, dont_allow_duplicate_keys_in_same_node)
{
    struct ptree_t* tree = ptree_create(NULL);

    EXPECT_THAT(ptree_add_node(tree, "node", NULL), NotNull());
    EXPECT_THAT(ptree_add_node(tree, "node", NULL), IsNull());
    EXPECT_THAT(map_count(&tree->children), Eq(1));

    ptree_destroy(tree, 0);
}

TEST(NAME, dont_allow_duplicate_keys_with_fill_in)
{
    struct ptree_t* tree = ptree_create(NULL);

    EXPECT_THAT(ptree_add_node(tree, "node1.node2.node3", NULL), NotNull());
    EXPECT_THAT(ptree_add_node(tree, "node1.node2.node3", NULL), IsNull());
    EXPECT_THAT(map_count(&tree->children), Eq(1));

    ptree_destroy(tree, 0);
}

TEST(NAME, recursively_destroy_tree_and_free_values)
{
    NiceMock<MockTreeItem> a;
    NiceMock<MockTreeItem> b;
    NiceMock<MockTreeItem> c;
    NiceMock<MockTreeItem> d;
    NiceMock<MockTreeItem> e;
    NiceMock<MockTreeItem> f;

    EXPECT_CALL(a, notifyDie()).Times(1);
    EXPECT_CALL(b, notifyDie()).Times(1);
    EXPECT_CALL(c, notifyDie()).Times(1);
    EXPECT_CALL(d, notifyDie()).Times(1);
    EXPECT_CALL(e, notifyDie()).Times(1);
    EXPECT_CALL(f, notifyDie()).Times(1);

    EXPECT_CALL(a, notifyCopy()).Times(0);
    EXPECT_CALL(b, notifyCopy()).Times(0);
    EXPECT_CALL(c, notifyCopy()).Times(0);
    EXPECT_CALL(d, notifyCopy()).Times(0);
    EXPECT_CALL(e, notifyCopy()).Times(0);
    EXPECT_CALL(f, notifyCopy()).Times(0);

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
    NiceMock<MockTreeItem> a;
    NiceMock<MockTreeItem> b;
    NiceMock<MockTreeItem> c;
    NiceMock<MockTreeItem> d;
    NiceMock<MockTreeItem> e;
    NiceMock<MockTreeItem> f;

    EXPECT_CALL(a, notifyDie()).Times(1);
    EXPECT_CALL(b, notifyDie()).Times(1);
    EXPECT_CALL(c, notifyDie()).Times(1);
    EXPECT_CALL(d, notifyDie()).Times(1);
    /* Note - Expecting this to never be called, as its free function will be missing */
    EXPECT_CALL(e, notifyDie()).Times(0);
    EXPECT_CALL(f, notifyDie()).Times(1);

    EXPECT_CALL(a, notifyCopy()).Times(0);
    EXPECT_CALL(b, notifyCopy()).Times(0);
    EXPECT_CALL(c, notifyCopy()).Times(0);
    EXPECT_CALL(d, notifyCopy()).Times(0);
    EXPECT_CALL(e, notifyCopy()).Times(0);
    EXPECT_CALL(f, notifyCopy()).Times(0);

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
    ptree_set_free_func(node5, NULL); // missing free function
    ptree_set_free_func(node6, (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node7, (ptree_free_func)mock_fake_delete_tree_item);

    ptree_destroy(tree, 1);
}

TEST(NAME, recursively_destroy_node)
{
    NiceMock<MockTreeItem> a;
    NiceMock<MockTreeItem> b;
    NiceMock<MockTreeItem> c;
    NiceMock<MockTreeItem> d;
    NiceMock<MockTreeItem> e;
    NiceMock<MockTreeItem> f;

    // only mock the free functions that are affected by destroying node1
    EXPECT_CALL(a, notifyDie()).Times(0);
    EXPECT_CALL(b, notifyDie()).Times(1);
    EXPECT_CALL(c, notifyDie()).Times(1);
    EXPECT_CALL(d, notifyDie()).Times(1);
    EXPECT_CALL(e, notifyDie()).Times(1);
    EXPECT_CALL(f, notifyDie()).Times(0);

    EXPECT_CALL(a, notifyCopy()).Times(0);
    EXPECT_CALL(b, notifyCopy()).Times(0);
    EXPECT_CALL(c, notifyCopy()).Times(0);
    EXPECT_CALL(d, notifyCopy()).Times(0);
    EXPECT_CALL(e, notifyCopy()).Times(0);
    EXPECT_CALL(f, notifyCopy()).Times(0);

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

    ptree_set_free_func(tree,  (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node1, (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node2, (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node3, (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node4, (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node5, (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node6, (ptree_free_func)mock_fake_delete_tree_item);
    ptree_set_free_func(node7, (ptree_free_func)mock_fake_delete_tree_item);

    // destroy node 1 and expect node 2, 3, 4, and 5 to go with it
    ptree_destroy(node1, 1);

    // check container sizes of remaining nodes
    EXPECT_THAT(map_count(&tree->children),  Eq(2));
    EXPECT_THAT(map_count(&node6->children), Eq(0));
    EXPECT_THAT(map_count(&node7->children), Eq(0));

    // check structure
    EXPECT_THAT((struct ptree_t*)map_find(&tree->children,  node1_hash), IsNull());
    EXPECT_THAT((struct ptree_t*)map_find(&tree->children,  node6_hash), Eq(node6));
    EXPECT_THAT((struct ptree_t*)map_find(&tree->children,  node7_hash), Eq(node7));

    // check parents
    EXPECT_THAT(tree->parent, IsNull());
    EXPECT_THAT(node6->parent, Eq(tree));
    EXPECT_THAT(node7->parent, Eq(tree));

    ptree_destroy(tree, 0);
}

TEST(NAME, duplicate_tree_succeeds_if_value_is_null_and_dup_and_free_func_are_not_set)
{
    struct ptree_t* tree = ptree_create(NULL);
    struct ptree_t* dup = ptree_duplicate_tree(tree);
    EXPECT_THAT(dup, NotNull());
    ptree_destroy(dup, 0);
    ptree_destroy(tree, 0);
}

TEST(NAME, duplicate_tree_fails_if_dup_and_free_func_are_not_set_and_value_is_not_null)
{
    int a = 7;
    struct ptree_t* tree = ptree_create(&a);
    EXPECT_THAT(ptree_duplicate_tree(tree), IsNull());
    ptree_destroy(tree, 0);
}

TEST(NAME, duplicate_tree_fails_if_dup_func_is_not_set_and_value_is_not_null)
{
    int a = 7;
    struct ptree_t* tree = ptree_create(&a);
    ptree_set_free_func(tree, (ptree_free_func)mock_fake_delete_tree_item);
    EXPECT_THAT(ptree_duplicate_tree(tree), IsNull());
    ptree_destroy(tree, 0);
}

TEST(NAME, duplicate_tree_fails_if_free_func_is_not_set_and_value_is_not_null)
{
    int a = 5;
    struct ptree_t* tree = ptree_create(&a);
    ptree_set_dup_func(tree, (ptree_dup_func)mock_dup_tree_item);
    EXPECT_THAT(ptree_duplicate_tree(tree), IsNull());
    ptree_destroy(tree, 0);
}

TEST(NAME, duplicate_tree)
{
    NiceMock<MockTreeItem> *a, *b, *c, *d, *e, *f;
    a = new NiceMock<MockTreeItem>;
    b = new NiceMock<MockTreeItem>;
    c = new NiceMock<MockTreeItem>;
    d = new NiceMock<MockTreeItem>;
    e = new NiceMock<MockTreeItem>;
    f = new NiceMock<MockTreeItem>;

    EXPECT_CALL(*a, notifyCopy()).Times(1);
    EXPECT_CALL(*b, notifyCopy()).Times(1);
    EXPECT_CALL(*c, notifyCopy()).Times(1);
    EXPECT_CALL(*d, notifyCopy()).Times(1);
    EXPECT_CALL(*e, notifyCopy()).Times(1);
    EXPECT_CALL(*f, notifyCopy()).Times(1);

    EXPECT_CALL(*a, notifyDie()).Times(0);
    EXPECT_CALL(*b, notifyDie()).Times(0);
    EXPECT_CALL(*c, notifyDie()).Times(0);
    EXPECT_CALL(*d, notifyDie()).Times(0);
    EXPECT_CALL(*e, notifyDie()).Times(0);
    EXPECT_CALL(*f, notifyDie()).Times(0);

    // root         (a)
    // |_node1      (b)
    // | |_node2    (null)
    // | |_node3    (c)
    // | | |_node4  (d)
    // | |_node5    (e)
    // |_node6      (null)
    // |_node7      (f)
    struct ptree_t* tree  = ptree_create(a);
    struct ptree_t* node1 = ptree_add_node(tree,  "node1", b);
    struct ptree_t* node2 = ptree_add_node(node1, "node2", NULL);
    struct ptree_t* node3 = ptree_add_node(node1, "node3", c);
    struct ptree_t* node4 = ptree_add_node(node3, "node4", d);
    struct ptree_t* node5 = ptree_add_node(node1, "node5", e);
    struct ptree_t* node6 = ptree_add_node(tree,  "node6", NULL);
    struct ptree_t* node7 = ptree_add_node(tree,  "node7", f);

    ptree_set_free_func(tree,  (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node1, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node3, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node4, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node5, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node7, (ptree_free_func)mock_free_tree_item);

    ptree_set_dup_func(tree,  (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node1, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node3, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node4, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node5, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node7, (ptree_dup_func)mock_dup_tree_item);

    // duplicate the tree
    struct ptree_t* dup = ptree_duplicate_tree(tree);
    struct ptree_t* dnode1 = ptree_get_node_no_depth(dup, "node1");
    struct ptree_t* dnode2 = ptree_get_node_no_depth(dnode1, "node2");
    struct ptree_t* dnode3 = ptree_get_node_no_depth(dnode1, "node3");
    struct ptree_t* dnode4 = ptree_get_node_no_depth(dnode3, "node4");
    struct ptree_t* dnode5 = ptree_get_node_no_depth(dnode1, "node5");
    struct ptree_t* dnode6 = ptree_get_node_no_depth(dup, "node6");
    struct ptree_t* dnode7 = ptree_get_node_no_depth(dup, "node7");

    uint32_t root_hash  = PTREE_HASH_STRING("root");
    uint32_t node1_hash = PTREE_HASH_STRING("node1");
    uint32_t node2_hash = PTREE_HASH_STRING("node2");
    uint32_t node3_hash = PTREE_HASH_STRING("node3");
    uint32_t node4_hash = PTREE_HASH_STRING("node4");
    uint32_t node5_hash = PTREE_HASH_STRING("node5");
    uint32_t node6_hash = PTREE_HASH_STRING("node6");
    uint32_t node7_hash = PTREE_HASH_STRING("node7");

    // make sure all nodes were copied
    EXPECT_THAT(dup, NotNull());
    EXPECT_THAT(dnode1, NotNull());
    EXPECT_THAT(dnode2, NotNull());
    EXPECT_THAT(dnode3, NotNull());
    EXPECT_THAT(dnode4, NotNull());
    EXPECT_THAT(dnode5, NotNull());
    EXPECT_THAT(dnode6, NotNull());
    EXPECT_THAT(dnode7, NotNull());

    // check container sizes
    EXPECT_THAT(map_count(&dup->children),  Eq(3));
    EXPECT_THAT(map_count(&dnode1->children), Eq(3));
    EXPECT_THAT(map_count(&dnode2->children), Eq(0));
    EXPECT_THAT(map_count(&dnode3->children), Eq(1));
    EXPECT_THAT(map_count(&dnode4->children), Eq(0));
    EXPECT_THAT(map_count(&dnode5->children), Eq(0));
    EXPECT_THAT(map_count(&dnode6->children), Eq(0));
    EXPECT_THAT(map_count(&dnode7->children), Eq(0));

    // check structure
    EXPECT_THAT((struct ptree_t*)map_find(&dup->children,  node1_hash), Eq(dnode1));
    EXPECT_THAT((struct ptree_t*)map_find(&dnode1->children, node2_hash), Eq(dnode2));
    EXPECT_THAT((struct ptree_t*)map_find(&dnode1->children, node3_hash), Eq(dnode3));
    EXPECT_THAT((struct ptree_t*)map_find(&dnode3->children, node4_hash), Eq(dnode4));
    EXPECT_THAT((struct ptree_t*)map_find(&dnode1->children, node5_hash), Eq(dnode5));
    EXPECT_THAT((struct ptree_t*)map_find(&dup->children,  node6_hash), Eq(dnode6));
    EXPECT_THAT((struct ptree_t*)map_find(&dup->children,  node7_hash), Eq(dnode7));

    // make sure none of the nodes are the same as the original tree
    EXPECT_THAT(dup, Ne(tree));
    EXPECT_THAT(node1, Ne(dnode1));
    EXPECT_THAT(node2, Ne(dnode2));
    EXPECT_THAT(node3, Ne(dnode3));
    EXPECT_THAT(node4, Ne(dnode4));
    EXPECT_THAT(node5, Ne(dnode5));
    EXPECT_THAT(node6, Ne(dnode6));
    EXPECT_THAT(node7, Ne(dnode7));

    // check parents
    EXPECT_THAT(dup->parent, IsNull());
    EXPECT_THAT(dnode1->parent, Eq(dup));
    EXPECT_THAT(dnode2->parent, Eq(dnode1));
    EXPECT_THAT(dnode3->parent, Eq(dnode1));
    EXPECT_THAT(dnode4->parent, Eq(dnode3));
    EXPECT_THAT(dnode5->parent, Eq(dnode1));
    EXPECT_THAT(dnode6->parent, Eq(dup));
    EXPECT_THAT(dnode7->parent, Eq(dup));

    // check values
    EXPECT_THAT(dup->value, AllOf(NotNull(), Ne(tree->value)));
    EXPECT_THAT(dnode1->value, AllOf(NotNull(), Ne(node1->value)));
    EXPECT_THAT(dnode2->value, IsNull());
    EXPECT_THAT(dnode3->value, AllOf(NotNull(), Ne(node3->value)));
    EXPECT_THAT(dnode4->value, AllOf(NotNull(), Ne(node4->value)));
    EXPECT_THAT(dnode5->value, AllOf(NotNull(), Ne(node5->value)));
    EXPECT_THAT(dnode6->value, IsNull());
    EXPECT_THAT(dnode7->value, AllOf(NotNull(), Ne(node7->value)));

    ptree_destroy(dup, 1);
    ptree_destroy(tree, 1);
}

TEST(NAME, duplicate_tree_node)
{
    NiceMock<MockTreeItem> *a, *b, *c, *d, *e, *f;
    a = new NiceMock<MockTreeItem>;
    b = new NiceMock<MockTreeItem>;
    c = new NiceMock<MockTreeItem>;
    d = new NiceMock<MockTreeItem>;
    e = new NiceMock<MockTreeItem>;
    f = new NiceMock<MockTreeItem>;

    // copying node 1 recursively
    EXPECT_CALL(*b, notifyCopy()).Times(1);
    EXPECT_CALL(*c, notifyCopy()).Times(1);
    EXPECT_CALL(*d, notifyCopy()).Times(1);
    EXPECT_CALL(*e, notifyCopy()).Times(1);

    // root         (a)
    // |_node1      (b)
    // | |_node2    (null)
    // | |_node3    (c)
    // | | |_node4  (d)
    // | |_node5    (e)
    // |_node6      (null)
    // |_node7      (f)
    struct ptree_t* tree  = ptree_create(a);
    struct ptree_t* node1 = ptree_add_node(tree,  "node1", b);
    struct ptree_t* node2 = ptree_add_node(node1, "node2", NULL);
    struct ptree_t* node3 = ptree_add_node(node1, "node3", c);
    struct ptree_t* node4 = ptree_add_node(node3, "node4", d);
    struct ptree_t* node5 = ptree_add_node(node1, "node5", e);
    struct ptree_t* node6 = ptree_add_node(tree,  "node6", NULL);
    struct ptree_t* node7 = ptree_add_node(tree,  "node7", f);

    ptree_set_free_func(tree,  (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node1, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node3, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node4, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node5, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node7, (ptree_free_func)mock_free_tree_item);

    ptree_set_dup_func(tree,  (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node1, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node3, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node4, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node5, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node7, (ptree_dup_func)mock_dup_tree_item);

    // duplicate node 1
    struct ptree_t* dnode1 = ptree_duplicate_tree(node1);
    struct ptree_t* dnode2 = ptree_get_node_no_depth(dnode1, "node2");
    struct ptree_t* dnode3 = ptree_get_node_no_depth(dnode1, "node3");
    struct ptree_t* dnode4 = ptree_get_node_no_depth(dnode3, "node4");
    struct ptree_t* dnode5 = ptree_get_node_no_depth(dnode1, "node5");

    uint32_t node1_hash = PTREE_HASH_STRING("node1");
    uint32_t node2_hash = PTREE_HASH_STRING("node2");
    uint32_t node3_hash = PTREE_HASH_STRING("node3");
    uint32_t node4_hash = PTREE_HASH_STRING("node4");
    uint32_t node5_hash = PTREE_HASH_STRING("node5");

    // make sure all nodes were copied
    EXPECT_THAT(dnode1, NotNull());
    EXPECT_THAT(dnode2, NotNull());
    EXPECT_THAT(dnode3, NotNull());
    EXPECT_THAT(dnode4, NotNull());
    EXPECT_THAT(dnode5, NotNull());

    // check container sizes
    EXPECT_THAT(map_count(&dnode1->children), Eq(3));
    EXPECT_THAT(map_count(&dnode2->children), Eq(0));
    EXPECT_THAT(map_count(&dnode3->children), Eq(1));
    EXPECT_THAT(map_count(&dnode4->children), Eq(0));
    EXPECT_THAT(map_count(&dnode5->children), Eq(0));

    // check structure
    EXPECT_THAT((struct ptree_t*)map_find(&dnode1->children, node2_hash), Eq(dnode2));
    EXPECT_THAT((struct ptree_t*)map_find(&dnode1->children, node3_hash), Eq(dnode3));
    EXPECT_THAT((struct ptree_t*)map_find(&dnode3->children, node4_hash), Eq(dnode4));
    EXPECT_THAT((struct ptree_t*)map_find(&dnode1->children, node5_hash), Eq(dnode5));

    // make sure none of the nodes are the same as the original tree
    EXPECT_THAT(node1, Ne(dnode1));
    EXPECT_THAT(node2, Ne(dnode2));
    EXPECT_THAT(node3, Ne(dnode3));
    EXPECT_THAT(node4, Ne(dnode4));
    EXPECT_THAT(node5, Ne(dnode5));

    // check parents
    EXPECT_THAT(dnode1->parent, IsNull());
    EXPECT_THAT(dnode2->parent, Eq(dnode1));
    EXPECT_THAT(dnode3->parent, Eq(dnode1));
    EXPECT_THAT(dnode4->parent, Eq(dnode3));
    EXPECT_THAT(dnode5->parent, Eq(dnode1));

    // check values
    EXPECT_THAT(dnode1->value, AllOf(NotNull(), Ne(node1->value)));
    EXPECT_THAT(dnode2->value, IsNull());
    EXPECT_THAT(dnode3->value, AllOf(NotNull(), Ne(node3->value)));
    EXPECT_THAT(dnode4->value, AllOf(NotNull(), Ne(node4->value)));
    EXPECT_THAT(dnode5->value, AllOf(NotNull(), Ne(node5->value)));

    ptree_destroy(dnode1, 1);
    ptree_destroy(tree, 1);
}

TEST(NAME, duplicate_tree_with_missing_dup_func_fails_and_cleans_up)
{
    NiceMock<MockTreeItem> *a, *b, *c, *d, *e, *f;
    a = new NiceMock<MockTreeItem>;
    b = new NiceMock<MockTreeItem>;
    c = new NiceMock<MockTreeItem>;
    d = new NiceMock<MockTreeItem>;
    e = new NiceMock<MockTreeItem>;
    f = new NiceMock<MockTreeItem>;

    EXPECT_CALL(*a, notifyCopy()).Times(1);
    EXPECT_CALL(*e, notifyCopy()).Times(0); // this is the missing dup function

    // We can't really set any more copy expectations on the other elements,
    // because the order in which they are copied is undefined.

    EXPECT_CALL(*a, notifyDie()).Times(1);
    EXPECT_CALL(*b, notifyDie()).Times(1);
    EXPECT_CALL(*c, notifyDie()).Times(1);
    EXPECT_CALL(*d, notifyDie()).Times(1);
    EXPECT_CALL(*e, notifyDie()).Times(1);
    EXPECT_CALL(*f, notifyDie()).Times(1);

    // root         (a)
    // |_node1      (b)
    // | |_node2    (null)
    // | |_node3    (c)
    // | | |_node4  (d)
    // | |_node5    (e)
    // |_node6      (null)
    // |_node7      (f)
    struct ptree_t* tree  = ptree_create(a);
    struct ptree_t* node1 = ptree_add_node(tree,  "node1", b);
    struct ptree_t* node2 = ptree_add_node(node1, "node2", NULL);
    struct ptree_t* node3 = ptree_add_node(node1, "node3", c);
    struct ptree_t* node4 = ptree_add_node(node3, "node4", d);
    struct ptree_t* node5 = ptree_add_node(node1, "node5", e);
    struct ptree_t* node6 = ptree_add_node(tree,  "node6", NULL);
    struct ptree_t* node7 = ptree_add_node(tree,  "node7", f);

    ptree_set_free_func(tree,  (ptree_free_func)mock_free_tree_item_and_notify);
    ptree_set_free_func(node1, (ptree_free_func)mock_free_tree_item_and_notify);
    ptree_set_free_func(node3, (ptree_free_func)mock_free_tree_item_and_notify);
    ptree_set_free_func(node4, (ptree_free_func)mock_free_tree_item_and_notify);
    ptree_set_free_func(node5, (ptree_free_func)mock_free_tree_item_and_notify);
    ptree_set_free_func(node7, (ptree_free_func)mock_free_tree_item_and_notify);

    ptree_set_dup_func(tree,  (ptree_dup_func)mock_dup_tree_item_and_expect_die);
    ptree_set_dup_func(node1, (ptree_dup_func)mock_dup_tree_item_and_expect_die);
    ptree_set_dup_func(node3, (ptree_dup_func)mock_dup_tree_item_and_expect_die);
    ptree_set_dup_func(node4, (ptree_dup_func)mock_dup_tree_item_and_expect_die);
    // node5 has missing dup function
    ptree_set_dup_func(node7, (ptree_dup_func)mock_dup_tree_item_and_expect_die);

    // copying should fail
    EXPECT_THAT(ptree_duplicate_tree(tree), IsNull());

    ptree_destroy(tree, 1);
}

TEST(NAME, duplicate_children_into_existing_empty_node)
{
    NiceMock<MockTreeItem> *a, *b, *c, *d, *e, *f;
    a = new NiceMock<MockTreeItem>;
    b = new NiceMock<MockTreeItem>;
    c = new NiceMock<MockTreeItem>;
    d = new NiceMock<MockTreeItem>;
    e = new NiceMock<MockTreeItem>;
    f = new NiceMock<MockTreeItem>;

    EXPECT_CALL(*a, notifyCopy()).Times(0); /* root node isn't copied */
    EXPECT_CALL(*b, notifyCopy()).Times(1);
    EXPECT_CALL(*c, notifyCopy()).Times(1);
    EXPECT_CALL(*d, notifyCopy()).Times(1);
    EXPECT_CALL(*e, notifyCopy()).Times(1);
    EXPECT_CALL(*f, notifyCopy()).Times(1);

    EXPECT_CALL(*a, notifyDie()).Times(0);
    EXPECT_CALL(*b, notifyDie()).Times(0);
    EXPECT_CALL(*c, notifyDie()).Times(0);
    EXPECT_CALL(*d, notifyDie()).Times(0);
    EXPECT_CALL(*e, notifyDie()).Times(0);
    EXPECT_CALL(*f, notifyDie()).Times(0);

    // root         (a)
    // |_node1      (b)
    // | |_node2    (null)
    // | |_node3    (c)
    // | | |_node4  (d)
    // | |_node5    (e)
    // |_node6      (null)
    // |_node7      (f)
    struct ptree_t* tree  = ptree_create(a);
    struct ptree_t* node1 = ptree_add_node(tree,  "node1", b);
    struct ptree_t* node2 = ptree_add_node(node1, "node2", NULL);
    struct ptree_t* node3 = ptree_add_node(node1, "node3", c);
    struct ptree_t* node4 = ptree_add_node(node3, "node4", d);
    struct ptree_t* node5 = ptree_add_node(node1, "node5", e);
    struct ptree_t* node6 = ptree_add_node(tree,  "node6", NULL);
    struct ptree_t* node7 = ptree_add_node(tree,  "node7", f);

    ptree_set_free_func(tree,  (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node1, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node3, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node4, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node5, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node7, (ptree_free_func)mock_free_tree_item);

    ptree_set_dup_func(tree,  (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node1, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node3, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node4, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node5, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node7, (ptree_dup_func)mock_dup_tree_item);

    // duplicate the tree into node 4, as that node is empty
    EXPECT_THAT(ptree_duplicate_children_into_existing_node(node4, tree), Ne(0));
    struct ptree_t* dnode1 = ptree_get_node_no_depth(node4, "node1");
    struct ptree_t* dnode2 = ptree_get_node_no_depth(dnode1, "node2");
    struct ptree_t* dnode3 = ptree_get_node_no_depth(dnode1, "node3");
    struct ptree_t* dnode4 = ptree_get_node_no_depth(dnode3, "node4");
    struct ptree_t* dnode5 = ptree_get_node_no_depth(dnode1, "node5");
    struct ptree_t* dnode6 = ptree_get_node_no_depth(node4, "node6");
    struct ptree_t* dnode7 = ptree_get_node_no_depth(node4, "node7");

    // tree now looks like this
    // root                 (a)
    // |_node1              (b)
    // | |_node2            (null)
    // | |_node3            (c)
    // | | |_node4          (d)        ____
    // | |   |_node1        (b)            |
    // | |   | |_node2      (null)         |
    // | |   | |_node3      (c)            |
    // | |   | | |_node4    (d)            |  this part is duplicated
    // | |   | |_node5      (e)            |
    // | |   |_node6        (null)         |
    // | |   |_node7        (f)        ____|
    // | |_node5            (e)
    // |_node6              (null)
    // |_node7              (f)

    uint32_t root_hash  = PTREE_HASH_STRING("root");
    uint32_t node1_hash = PTREE_HASH_STRING("node1");
    uint32_t node2_hash = PTREE_HASH_STRING("node2");
    uint32_t node3_hash = PTREE_HASH_STRING("node3");
    uint32_t node4_hash = PTREE_HASH_STRING("node4");
    uint32_t node5_hash = PTREE_HASH_STRING("node5");
    uint32_t node6_hash = PTREE_HASH_STRING("node6");
    uint32_t node7_hash = PTREE_HASH_STRING("node7");

    // make sure all nodes were copied
    EXPECT_THAT(dnode1, NotNull());
    EXPECT_THAT(dnode2, NotNull());
    EXPECT_THAT(dnode3, NotNull());
    EXPECT_THAT(dnode4, NotNull());
    EXPECT_THAT(dnode5, NotNull());
    EXPECT_THAT(dnode6, NotNull());
    EXPECT_THAT(dnode7, NotNull());

    // check container sizes
    EXPECT_THAT(map_count(&tree->children),  Eq(3));
    EXPECT_THAT(map_count(&node1->children), Eq(3));
    EXPECT_THAT(map_count(&node2->children), Eq(0));
    EXPECT_THAT(map_count(&node3->children), Eq(1));
    EXPECT_THAT(map_count(&node4->children), Eq(3));
    EXPECT_THAT(map_count(&node5->children), Eq(0));
    EXPECT_THAT(map_count(&node6->children), Eq(0));
    EXPECT_THAT(map_count(&node7->children), Eq(0));
    EXPECT_THAT(map_count(&dnode1->children), Eq(3));
    EXPECT_THAT(map_count(&dnode2->children), Eq(0));
    EXPECT_THAT(map_count(&dnode3->children), Eq(1));
    EXPECT_THAT(map_count(&dnode4->children), Eq(0));
    EXPECT_THAT(map_count(&dnode5->children), Eq(0));
    EXPECT_THAT(map_count(&dnode6->children), Eq(0));
    EXPECT_THAT(map_count(&dnode7->children), Eq(0));

    // check structure
    EXPECT_THAT((struct ptree_t*)map_find(&tree->children,  node1_hash), Eq(node1));
    EXPECT_THAT((struct ptree_t*)map_find(&node1->children, node2_hash), Eq(node2));
    EXPECT_THAT((struct ptree_t*)map_find(&node1->children, node3_hash), Eq(node3));
    EXPECT_THAT((struct ptree_t*)map_find(&node3->children, node4_hash), Eq(node4));
    EXPECT_THAT((struct ptree_t*)map_find(&node1->children, node5_hash), Eq(node5));
    EXPECT_THAT((struct ptree_t*)map_find(&tree->children,  node6_hash), Eq(node6));
    EXPECT_THAT((struct ptree_t*)map_find(&tree->children,  node7_hash), Eq(node7));
    EXPECT_THAT((struct ptree_t*)map_find(&node4->children, node1_hash), Eq(dnode1));
    EXPECT_THAT((struct ptree_t*)map_find(&dnode1->children, node2_hash), Eq(dnode2));
    EXPECT_THAT((struct ptree_t*)map_find(&dnode1->children, node3_hash), Eq(dnode3));
    EXPECT_THAT((struct ptree_t*)map_find(&dnode3->children, node4_hash), Eq(dnode4));
    EXPECT_THAT((struct ptree_t*)map_find(&dnode1->children, node5_hash), Eq(dnode5));
    EXPECT_THAT((struct ptree_t*)map_find(&node4->children, node6_hash), Eq(dnode6));
    EXPECT_THAT((struct ptree_t*)map_find(&node4->children, node7_hash), Eq(dnode7));

    // make sure none of the nodes are the same as the original tree
    EXPECT_THAT(node1, Ne(dnode1));
    EXPECT_THAT(node2, Ne(dnode2));
    EXPECT_THAT(node3, Ne(dnode3));
    EXPECT_THAT(node4, Ne(dnode4));
    EXPECT_THAT(node5, Ne(dnode5));
    EXPECT_THAT(node6, Ne(dnode6));
    EXPECT_THAT(node7, Ne(dnode7));

    // check parents
    EXPECT_THAT(tree->parent, IsNull());
    EXPECT_THAT(node1->parent, Eq(tree));
    EXPECT_THAT(node2->parent, Eq(node1));
    EXPECT_THAT(node3->parent, Eq(node1));
    EXPECT_THAT(node4->parent, Eq(node3));
    EXPECT_THAT(node5->parent, Eq(node1));
    EXPECT_THAT(node6->parent, Eq(tree));
    EXPECT_THAT(node7->parent, Eq(tree));
    EXPECT_THAT(dnode1->parent, Eq(node4));
    EXPECT_THAT(dnode2->parent, Eq(dnode1));
    EXPECT_THAT(dnode3->parent, Eq(dnode1));
    EXPECT_THAT(dnode4->parent, Eq(dnode3));
    EXPECT_THAT(dnode5->parent, Eq(dnode1));
    EXPECT_THAT(dnode6->parent, Eq(node4));
    EXPECT_THAT(dnode7->parent, Eq(node4));

    // check values
    EXPECT_THAT(dnode1->value, AllOf(NotNull(), Ne(node1->value)));
    EXPECT_THAT(dnode2->value, IsNull());
    EXPECT_THAT(dnode3->value, AllOf(NotNull(), Ne(node3->value)));
    EXPECT_THAT(dnode4->value, AllOf(NotNull(), Ne(node4->value)));
    EXPECT_THAT(dnode5->value, AllOf(NotNull(), Ne(node5->value)));
    EXPECT_THAT(dnode6->value, IsNull());
    EXPECT_THAT(dnode7->value, AllOf(NotNull(), Ne(node7->value)));

    ptree_destroy(tree, 1);
}

TEST(NAME, duplicate_tree_into_existing_node_with_children)
{
    NiceMock<MockTreeItem> *a, *b, *c, *d, *e, *f;
    a = new NiceMock<MockTreeItem>;
    b = new NiceMock<MockTreeItem>;
    c = new NiceMock<MockTreeItem>;
    d = new NiceMock<MockTreeItem>;
    e = new NiceMock<MockTreeItem>;
    f = new NiceMock<MockTreeItem>;

    EXPECT_CALL(*a, notifyCopy()).Times(0);
    EXPECT_CALL(*b, notifyCopy()).Times(1);
    EXPECT_CALL(*c, notifyCopy()).Times(1);
    EXPECT_CALL(*d, notifyCopy()).Times(1);
    EXPECT_CALL(*e, notifyCopy()).Times(1);
    EXPECT_CALL(*f, notifyCopy()).Times(1);

    EXPECT_CALL(*a, notifyDie()).Times(0);
    EXPECT_CALL(*b, notifyDie()).Times(0);
    EXPECT_CALL(*c, notifyDie()).Times(0);
    EXPECT_CALL(*d, notifyDie()).Times(0);
    EXPECT_CALL(*e, notifyDie()).Times(0);
    EXPECT_CALL(*f, notifyDie()).Times(0);

    // root         (a)
    // |_node1      (b)
    // | |_node2    (null)
    // | |_node3    (c)
    // | | |_node4  (d)
    // | |_node5    (e)
    // |_node6      (null)
    // |_node7      (f)
    struct ptree_t* tree  = ptree_create(a);
    struct ptree_t* node1 = ptree_add_node(tree,  "node1", b);
    struct ptree_t* node2 = ptree_add_node(node1, "node2", NULL);
    struct ptree_t* node3 = ptree_add_node(node1, "node3", c);
    struct ptree_t* node4 = ptree_add_node(node3, "node4", d);
    struct ptree_t* node5 = ptree_add_node(node1, "node5", e);
    struct ptree_t* node6 = ptree_add_node(tree,  "node6", NULL);
    struct ptree_t* node7 = ptree_add_node(tree,  "node7", f);

    ptree_set_free_func(tree,  (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node1, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node3, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node4, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node5, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node7, (ptree_free_func)mock_free_tree_item);

    ptree_set_dup_func(tree,  (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node1, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node3, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node4, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node5, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node7, (ptree_dup_func)mock_dup_tree_item);

    // duplicate the tree into node 1
    EXPECT_THAT(ptree_duplicate_children_into_existing_node(node1, tree), Ne(0));
    struct ptree_t* dnode1 = ptree_get_node_no_depth(node1, "node1");
    struct ptree_t* dnode2 = ptree_get_node_no_depth(dnode1, "node2");
    struct ptree_t* dnode3 = ptree_get_node_no_depth(dnode1, "node3");
    struct ptree_t* dnode4 = ptree_get_node_no_depth(dnode3, "node4");
    struct ptree_t* dnode5 = ptree_get_node_no_depth(dnode1, "node5");
    struct ptree_t* dnode6 = ptree_get_node_no_depth(node1, "node6");
    struct ptree_t* dnode7 = ptree_get_node_no_depth(node1, "node7");

    // tree now looks like this
    // root                 (a)
    // |_node1              (b)        ____
    // | |_node1            (b)            |
    // | | |_node2          (null)         |
    // | | |_node3          (c)            |
    // | | | |_node4        (d)            | this section was duplicated
    // | | |_node5          (e)            |
    // | |_node6            (null)         |
    // | |_node7            (f)        ____|
    // | |_node2            (null)
    // | |_node3            (c)
    // | | |_node4          (d)
    // | |_node5            (e)
    // |_node6              (null)
    // |_node7              (f)

    uint32_t root_hash  = PTREE_HASH_STRING("root");
    uint32_t node1_hash = PTREE_HASH_STRING("node1");
    uint32_t node2_hash = PTREE_HASH_STRING("node2");
    uint32_t node3_hash = PTREE_HASH_STRING("node3");
    uint32_t node4_hash = PTREE_HASH_STRING("node4");
    uint32_t node5_hash = PTREE_HASH_STRING("node5");
    uint32_t node6_hash = PTREE_HASH_STRING("node6");
    uint32_t node7_hash = PTREE_HASH_STRING("node7");

    // make sure all nodes were copied
    EXPECT_THAT(dnode1, NotNull());
    EXPECT_THAT(dnode2, NotNull());
    EXPECT_THAT(dnode3, NotNull());
    EXPECT_THAT(dnode4, NotNull());
    EXPECT_THAT(dnode5, NotNull());
    EXPECT_THAT(dnode6, NotNull());
    EXPECT_THAT(dnode7, NotNull());

    // check container sizes
    EXPECT_THAT(map_count(&tree->children),  Eq(3));
    EXPECT_THAT(map_count(&node1->children), Eq(6));
    EXPECT_THAT(map_count(&node2->children), Eq(0));
    EXPECT_THAT(map_count(&node3->children), Eq(1));
    EXPECT_THAT(map_count(&node4->children), Eq(0));
    EXPECT_THAT(map_count(&node5->children), Eq(0));
    EXPECT_THAT(map_count(&node6->children), Eq(0));
    EXPECT_THAT(map_count(&node7->children), Eq(0));
    EXPECT_THAT(map_count(&dnode1->children), Eq(3));
    EXPECT_THAT(map_count(&dnode2->children), Eq(0));
    EXPECT_THAT(map_count(&dnode3->children), Eq(1));
    EXPECT_THAT(map_count(&dnode4->children), Eq(0));
    EXPECT_THAT(map_count(&dnode5->children), Eq(0));
    EXPECT_THAT(map_count(&dnode6->children), Eq(0));
    EXPECT_THAT(map_count(&dnode7->children), Eq(0));

    // check structure
    EXPECT_THAT((struct ptree_t*)map_find(&tree->children,  node1_hash), Eq(node1));
    EXPECT_THAT((struct ptree_t*)map_find(&node1->children, node2_hash), Eq(node2));
    EXPECT_THAT((struct ptree_t*)map_find(&node1->children, node3_hash), Eq(node3));
    EXPECT_THAT((struct ptree_t*)map_find(&node3->children, node4_hash), Eq(node4));
    EXPECT_THAT((struct ptree_t*)map_find(&node1->children, node5_hash), Eq(node5));
    EXPECT_THAT((struct ptree_t*)map_find(&tree->children,  node6_hash), Eq(node6));
    EXPECT_THAT((struct ptree_t*)map_find(&tree->children,  node7_hash), Eq(node7));
    EXPECT_THAT((struct ptree_t*)map_find(&node1->children, node1_hash), Eq(dnode1));
    EXPECT_THAT((struct ptree_t*)map_find(&dnode1->children, node2_hash), Eq(dnode2));
    EXPECT_THAT((struct ptree_t*)map_find(&dnode1->children, node3_hash), Eq(dnode3));
    EXPECT_THAT((struct ptree_t*)map_find(&dnode3->children, node4_hash), Eq(dnode4));
    EXPECT_THAT((struct ptree_t*)map_find(&dnode1->children, node5_hash), Eq(dnode5));
    EXPECT_THAT((struct ptree_t*)map_find(&node1->children, node6_hash), Eq(dnode6));
    EXPECT_THAT((struct ptree_t*)map_find(&node1->children, node7_hash), Eq(dnode7));

    // make sure none of the nodes are the same as the original tree
    EXPECT_THAT(node1, Ne(dnode1));
    EXPECT_THAT(node2, Ne(dnode2));
    EXPECT_THAT(node3, Ne(dnode3));
    EXPECT_THAT(node4, Ne(dnode4));
    EXPECT_THAT(node5, Ne(dnode5));
    EXPECT_THAT(node6, Ne(dnode6));
    EXPECT_THAT(node7, Ne(dnode7));

    // check parents
    EXPECT_THAT(tree->parent, IsNull());
    EXPECT_THAT(node1->parent, Eq(tree));
    EXPECT_THAT(node2->parent, Eq(node1));
    EXPECT_THAT(node3->parent, Eq(node1));
    EXPECT_THAT(node4->parent, Eq(node3));
    EXPECT_THAT(node5->parent, Eq(node1));
    EXPECT_THAT(node6->parent, Eq(tree));
    EXPECT_THAT(node7->parent, Eq(tree));
    EXPECT_THAT(dnode1->parent, Eq(node1));
    EXPECT_THAT(dnode2->parent, Eq(dnode1));
    EXPECT_THAT(dnode3->parent, Eq(dnode1));
    EXPECT_THAT(dnode4->parent, Eq(dnode3));
    EXPECT_THAT(dnode5->parent, Eq(dnode1));
    EXPECT_THAT(dnode6->parent, Eq(node1));
    EXPECT_THAT(dnode7->parent, Eq(node1));

    // check values
    EXPECT_THAT(tree->value, NotNull());
    EXPECT_THAT(dnode1->value, AllOf(NotNull(), Ne(node1->value)));
    EXPECT_THAT(dnode2->value, IsNull());
    EXPECT_THAT(dnode3->value, AllOf(NotNull(), Ne(node3->value)));
    EXPECT_THAT(dnode4->value, AllOf(NotNull(), Ne(node4->value)));
    EXPECT_THAT(dnode5->value, AllOf(NotNull(), Ne(node5->value)));
    EXPECT_THAT(dnode6->value, IsNull());
    EXPECT_THAT(dnode7->value, AllOf(NotNull(), Ne(node7->value)));

    ptree_destroy(tree, 1);
}

TEST(NAME, duplicate_tree_into_itself_fails)
{
    NiceMock<MockTreeItem> *a, *b, *c, *d, *e, *f;
    a = new NiceMock<MockTreeItem>;
    b = new NiceMock<MockTreeItem>;
    c = new NiceMock<MockTreeItem>;
    d = new NiceMock<MockTreeItem>;
    e = new NiceMock<MockTreeItem>;
    f = new NiceMock<MockTreeItem>;

    EXPECT_CALL(*a, notifyCopy()).Times(0);
    // Can't really set any expectations on other elements being copied.
    // The implementation could do the check for duplicate keys before or
    // after tree duplication.

    EXPECT_CALL(*a, notifyDie()).Times(0);
    EXPECT_CALL(*b, notifyDie()).Times(0);
    EXPECT_CALL(*c, notifyDie()).Times(0);
    EXPECT_CALL(*d, notifyDie()).Times(0);
    EXPECT_CALL(*e, notifyDie()).Times(0);
    EXPECT_CALL(*f, notifyDie()).Times(0);

    // root         (a)
    // |_node1      (b)
    // | |_node2    (null)
    // | |_node3    (c)
    // | | |_node4  (d)
    // | |_node5    (e)
    // |_node6      (null)
    // |_node7      (f)
    struct ptree_t* tree  = ptree_create(a);
    struct ptree_t* node1 = ptree_add_node(tree,  "node1", b);
    struct ptree_t* node2 = ptree_add_node(node1, "node2", NULL);
    struct ptree_t* node3 = ptree_add_node(node1, "node3", c);
    struct ptree_t* node4 = ptree_add_node(node3, "node4", d);
    struct ptree_t* node5 = ptree_add_node(node1, "node5", e);
    struct ptree_t* node6 = ptree_add_node(tree,  "node6", NULL);
    struct ptree_t* node7 = ptree_add_node(tree,  "node7", f);

    ptree_set_free_func(tree,  (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node1, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node3, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node4, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node5, (ptree_free_func)mock_free_tree_item);
    ptree_set_free_func(node7, (ptree_free_func)mock_free_tree_item);

    ptree_set_dup_func(tree,  (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node1, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node3, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node4, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node5, (ptree_dup_func)mock_dup_tree_item);
    ptree_set_dup_func(node7, (ptree_dup_func)mock_dup_tree_item);

    // duplicate the tree into itself
    EXPECT_THAT(ptree_duplicate_children_into_existing_node(tree, tree), Eq(0));

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

    // check parents
    EXPECT_THAT(tree->parent, IsNull());
    EXPECT_THAT(node1->parent, Eq(tree));
    EXPECT_THAT(node2->parent, Eq(node1));
    EXPECT_THAT(node3->parent, Eq(node1));
    EXPECT_THAT(node4->parent, Eq(node3));
    EXPECT_THAT(node5->parent, Eq(node1));
    EXPECT_THAT(node6->parent, Eq(tree));
    EXPECT_THAT(node7->parent, Eq(tree));

    ptree_destroy(tree, 1);
}

TEST(NAME, relocate_node_in_tree)
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

    // relocate node3 to be a child of root
    EXPECT_THAT(ptree_set_parent(node3, tree, "node3"), Ne(0));

    // tree now looks like this
    // root         (a)
    // |_node1      (b)
    // | |_node2    (null)
    // | |_node5    (e)
    // |_node3      (c)
    // | |_node4    (d)
    // |_node6      (null)
    // |_node7      (f)

    uint32_t root_hash  = PTREE_HASH_STRING("root");
    uint32_t node1_hash = PTREE_HASH_STRING("node1");
    uint32_t node2_hash = PTREE_HASH_STRING("node2");
    uint32_t node3_hash = PTREE_HASH_STRING("node3");
    uint32_t node4_hash = PTREE_HASH_STRING("node4");
    uint32_t node5_hash = PTREE_HASH_STRING("node5");
    uint32_t node6_hash = PTREE_HASH_STRING("node6");
    uint32_t node7_hash = PTREE_HASH_STRING("node7");

    // check container sizes
    EXPECT_THAT(map_count(&tree->children),  Eq(4));
    EXPECT_THAT(map_count(&node1->children), Eq(2));
    EXPECT_THAT(map_count(&node2->children), Eq(0));
    EXPECT_THAT(map_count(&node3->children), Eq(1));
    EXPECT_THAT(map_count(&node4->children), Eq(0));
    EXPECT_THAT(map_count(&node5->children), Eq(0));
    EXPECT_THAT(map_count(&node6->children), Eq(0));
    EXPECT_THAT(map_count(&node7->children), Eq(0));

    // check structure
    EXPECT_THAT((struct ptree_t*)map_find(&tree->children,  node1_hash), Eq(node1));
    EXPECT_THAT((struct ptree_t*)map_find(&node1->children, node2_hash), Eq(node2));
    EXPECT_THAT((struct ptree_t*)map_find(&tree->children,  node3_hash), Eq(node3));
    EXPECT_THAT((struct ptree_t*)map_find(&node3->children, node4_hash), Eq(node4));
    EXPECT_THAT((struct ptree_t*)map_find(&node1->children, node5_hash), Eq(node5));
    EXPECT_THAT((struct ptree_t*)map_find(&tree->children,  node6_hash), Eq(node6));
    EXPECT_THAT((struct ptree_t*)map_find(&tree->children,  node7_hash), Eq(node7));

    // check parents
    EXPECT_THAT(tree->parent, IsNull());
    EXPECT_THAT(node1->parent, Eq(tree));
    EXPECT_THAT(node2->parent, Eq(node1));
    EXPECT_THAT(node3->parent, Eq(tree));
    EXPECT_THAT(node4->parent, Eq(node3));
    EXPECT_THAT(node5->parent, Eq(node1));
    EXPECT_THAT(node6->parent, Eq(tree));
    EXPECT_THAT(node7->parent, Eq(tree));

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

TEST(NAME, relocate_parent_node_into_child_node_fails)
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

    // none of these should work
    EXPECT_THAT(ptree_set_parent(tree, node1, "node1"), Eq(0));
    EXPECT_THAT(ptree_set_parent(tree, node4, "node4"), Eq(0));
    EXPECT_THAT(ptree_set_parent(node3, node4, "node3"), Eq(0));
    EXPECT_THAT(ptree_set_parent(node1, node5, "node1"), Eq(0));

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

    // check parents
    EXPECT_THAT(tree->parent, IsNull());
    EXPECT_THAT(node1->parent, Eq(tree));
    EXPECT_THAT(node2->parent, Eq(node1));
    EXPECT_THAT(node3->parent, Eq(node1));
    EXPECT_THAT(node4->parent, Eq(node3));
    EXPECT_THAT(node5->parent, Eq(node1));
    EXPECT_THAT(node6->parent, Eq(tree));
    EXPECT_THAT(node7->parent, Eq(tree));

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

TEST(NAME, get_node_no_depth)
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

    EXPECT_THAT(ptree_get_node_no_depth(tree, "node1"), Eq(node1));
    EXPECT_THAT(ptree_get_node_no_depth(tree, "node2"), IsNull());
    EXPECT_THAT(ptree_get_node_no_depth(node1, "node2"), Eq(node2));
    EXPECT_THAT(ptree_get_node_no_depth(node6, "root"), IsNull());

    ptree_destroy(tree, 0);
}

TEST(NAME, get_node_existing_key)
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

    EXPECT_THAT(ptree_get_node(tree, "node1.node3.node4"), Eq(node4));
    EXPECT_THAT(ptree_get_node(node1, "node2"), Eq(node2));
    EXPECT_THAT(ptree_get_node(node1, "node3.node4"), Eq(node4));

    ptree_destroy(tree, 0);
}

TEST(NAME, find_non_existing_key_in_tree)
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

    EXPECT_THAT(ptree_get_node(tree, "this.doesn't.exist"), IsNull());
    EXPECT_THAT(ptree_get_node(tree, "node1.node3.nope"), IsNull());

    ptree_destroy(tree, 0);
}

TEST(NAME, traverse_node_children)
{
    const char* keys[] = {"node1", "node2", "node3", "node4"};
    int values[] = {4, 2, 7, 5};
    struct ptree_t* tree = ptree_create(NULL);
    ptree_add_node(tree, keys[0], &values[0]);
    ptree_add_node(tree, keys[1], &values[1]);
    ptree_add_node(tree, keys[2], &values[2]);
    ptree_add_node(tree, keys[3], &values[3]);

    PTREE_FOR_EACH_IN_NODE(tree, hash, node)
    {
        int i;
        for(i = 0; i != 3; ++i)
            if(PTREE_HASH_STRING(keys[i]) == hash)
                break;
        ASSERT_LT(i, 4);
        EXPECT_THAT((int*)node->value, Pointee(values[i]));
    }

    ptree_destroy(tree, 0);
}
