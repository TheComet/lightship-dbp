#include "tests/globals.hpp"
#include "gmock/gmock.h"
#include "util/ptree.h"

#define NAME ptree_DeathTest

using namespace testing;

TEST(NAME, init_with_tree_as_null_ptr)
{
    ASSERT_DEATH(ptree_init_ptree(NULL, NULL), ASSERTION_REGEX);
}

TEST(NAME, destroy_with_tree_as_null_ptr)
{
    ASSERT_DEATH(ptree_destroy(NULL), ASSERTION_REGEX);
}

TEST(NAME, destroy_keep_root_with_tree_as_null_ptr)
{
    ASSERT_DEATH(ptree_destroy_keep_root(NULL), ASSERTION_REGEX);
}

TEST(NAME, add_node_with_tree_as_null_ptr)
{
    ASSERT_DEATH(ptree_set(NULL, "key", NULL), ASSERTION_REGEX);
}

TEST(NAME, add_node_with_key_as_null_ptr)
{
    struct ptree_t* tree = ptree_create(NULL);
    EXPECT_DEATH(ptree_set(tree, NULL, NULL), ASSERTION_REGEX);
    ptree_destroy(tree);
}

TEST(NAME, set_parent_with_node_as_null_ptr)
{
    struct ptree_t* tree = ptree_create(NULL);
    EXPECT_DEATH(ptree_set_parent(NULL, tree, "key"), ASSERTION_REGEX);
    ptree_destroy(tree);
}

TEST(NAME, set_parent_with_key_as_null_ptr)
{
    struct ptree_t* tree = ptree_create(NULL);
    EXPECT_DEATH(ptree_set_parent(tree, NULL, NULL), ASSERTION_REGEX);
    ptree_destroy(tree);
}

TEST(NAME, remove_node_with_node_as_null_ptr)
{
    ASSERT_DEATH(ptree_remove(NULL, "key"), ASSERTION_REGEX);
}

TEST(NAME, remove_node_with_key_as_null_ptr)
{
    struct ptree_t* tree = ptree_create(NULL);
    EXPECT_DEATH(ptree_remove(tree, NULL), ASSERTION_REGEX);
    ptree_destroy(tree);
}

TEST(NAME, clean_with_tree_as_null_ptr)
{
    ASSERT_DEATH(ptree_clean(NULL), ASSERTION_REGEX);
}

TEST(NAME, get_root_with_node_as_null_ptr)
{
    ASSERT_DEATH(ptree_get_root(NULL), ASSERTION_REGEX);
}

TEST(NAME, set_dup_func_with_node_as_null_ptr)
{
    ASSERT_DEATH(ptree_set_dup_func(NULL, NULL), ASSERTION_REGEX);
}

TEST(NAME, set_free_func_with_node_as_null_ptr)
{
    ASSERT_DEATH(ptree_set_free_func(NULL, NULL), ASSERTION_REGEX);
}

TEST(NAME, duplicate_children_into_existing_node_with_source_as_null_ptr)
{
    struct ptree_t* tree = ptree_create(NULL);
    EXPECT_DEATH(ptree_duplicate_children_into_existing_node(tree, NULL), ASSERTION_REGEX);
    ptree_destroy(tree);
}

TEST(NAME, duplicate_children_into_existing_node_with_target_as_null_ptr)
{
    struct ptree_t* tree = ptree_create(NULL);
    EXPECT_DEATH(ptree_duplicate_children_into_existing_node(NULL, tree), ASSERTION_REGEX);
    ptree_destroy(tree);
}

TEST(NAME, duplicate_tree_with_tree_as_null_ptr)
{
    ASSERT_DEATH(ptree_duplicate_tree(NULL), ASSERTION_REGEX);
}

TEST(NAME, get_node_no_depth_with_node_as_null_ptr)
{
    ASSERT_DEATH(ptree_get_node_no_depth(NULL, "key"), ASSERTION_REGEX);
}

TEST(NAME, get_node_no_depth_with_key_as_null_ptr)
{
    struct ptree_t* tree = ptree_create(NULL);
    ASSERT_DEATH(ptree_get_node_no_depth(tree, NULL), ASSERTION_REGEX);
    ptree_destroy(tree);
}

TEST(NAME, get_node_with_node_as_null_ptr)
{
    ASSERT_DEATH(ptree_get_node(NULL, "key"), ASSERTION_REGEX);
}

TEST(NAME, get_node_with_key_as_null_ptr)
{
    struct ptree_t* tree = ptree_create(NULL);
    ASSERT_DEATH(ptree_get_node(tree, NULL), ASSERTION_REGEX);
    ptree_destroy(tree);
}

TEST(NAME, node_is_child_of_with_tree_as_null_ptr)
{
    ASSERT_DEATH(ptree_node_is_child_of(NULL, NULL), ASSERTION_REGEX);
}
