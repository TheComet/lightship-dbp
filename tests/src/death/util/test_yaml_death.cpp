#include "tests/globals.hpp"
#include "gmock/gmock.h"
#include "util/yaml.h"

#define NAME yaml_DeathTest

TEST(NAME, load_filename_is_null)
{
    ASSERT_DEATH(yaml_load(NULL), ASSERTION_REGEX);
}

#if defined(LIGHTSHIP_UTIL_PLATFORM_LINUX) || defined(LIGHTSHIP_UTIL_PLATFORM_MACOSX)
TEST(NAME, load_from_memory_buffer_is_null)
{
    ASSERT_DEATH(yaml_load_from_memory(NULL), ASSERTION_REGEX);
}
#endif

TEST(NAME, load_from_stream_stream_is_null)
{
    ASSERT_DEATH(yaml_load_from_stream(NULL), ASSERTION_REGEX);
}

TEST(NAME, destroy_node_is_null)
{
    ASSERT_DEATH(yaml_destroy(NULL), ASSERTION_REGEX);
}

TEST(NAME, get_value_node_is_null)
{
    ASSERT_DEATH(yaml_get_value(NULL, "key"), ASSERTION_REGEX);
}

TEST(NAME, get_value_key_is_null)
{
    struct ptree_t* doc = yaml_create();
    EXPECT_DEATH(yaml_get_value(doc, NULL), ASSERTION_REGEX);
    yaml_destroy(doc);
}

TEST(NAME, get_node_node_is_null)
{
    ASSERT_DEATH(yaml_get_node(NULL, "key"), ASSERTION_REGEX);
}

TEST(NAME, get_node_key_is_null)
{
    struct ptree_t* doc = yaml_create();
    EXPECT_DEATH(yaml_get_node(doc, NULL), ASSERTION_REGEX);
    yaml_destroy(doc);
}

TEST(NAME, set_value_node_is_null)
{
    ASSERT_DEATH(yaml_set_value(NULL, "key", NULL), ASSERTION_REGEX);
}

TEST(NAME, set_value_key_is_null)
{
    struct ptree_t* doc = yaml_create();
    EXPECT_DEATH(yaml_set_value(doc, NULL, NULL), ASSERTION_REGEX);
    yaml_destroy(doc);
}

TEST(NAME, remove_value_node_is_null)
{
    ASSERT_DEATH(yaml_remove_value(NULL, "key"), ASSERTION_REGEX);
}

TEST(NAME, remove_value_key_is_null)
{
    struct ptree_t* doc = yaml_create();
    EXPECT_DEATH(yaml_remove_value(doc, NULL), ASSERTION_REGEX);
    yaml_destroy(doc);
}
