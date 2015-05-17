#include "gmock/gmock.h"
#include "util/yaml.h"
#include "util/ptree.h"
#include "util/config.h"

#define NAME yaml

using namespace testing;

static const char* bare_minimum_yml = "key:\n    key: value\n";

static const char* basic_yml =
"root:\n"
"    players:\n"
"        player1:\n"
"            name: Will Smith\n"
"            age: 200\n"
"            sex: female\n"
"        player2:\n"
"            name: TheComet\n"
"            age: 21\n"
"            sex: male\n"
"    enemies:\n"
"        enemy1:\n"
"            name: George Bush\n"
"            age: 9001\n"
"            sex: Who knows?\n"
"        enemy2:\n"
"            name: Big Daddy\n"
"            age: 394\n"
"            sex: dad\n";

static const char* anchor_yml =
"copyable_item:\n"
"    item1: value1\n"
"    item2: value2\n"
"some_thing:\n"
"    another_thing:\n"
"        values: *copyable_item\n"
"    another_thing2:\n"
"        values: *copyable_item\n";

static const char* anchor_same_names_yml =
"copyable_item:\n"
"    item1: value1\n"
"    item2: value2\n"
"thing:\n"
"    bar:\n"
"        items: *copyable_item\n"
"        items: *copyable_item\n"
"        items: *copyable_item\n";

static const char* lists1_yml =
"root:\n"
"    items:\n"
"      - item1: thing1\n"
"      - item2: thing2\n"
"      - item3: thing3\n";

static const char* lists2_yml =
"root:\n"
"    items:\n"
"      - thing1\n"
"      - thing2\n"
"      - thing3\n";

static const char* lists3_yml =
"root:\n"
"    items: [thing1, thing2, thing3]\n";

static const char* anchor_and_list_yml =
"item:\n"
"    key1: value1\n"
"    key2: value2\n"
"items:\n"
"  - *item\n"
"  - *item\n";

static const char* anchor_and_list2_yml =
"item:\n"
"    key1: value1\n"
"    key2: value2\n"
"items:\n"
"  - \n"
"    key1: value1\n"
"    key2: value2\n"
"  - \n"
"    key1: value1\n"
"    key2: value2\n";

TEST(NAME, create)
{
    struct ptree_t* doc;

    ASSERT_THAT((doc = yaml_create()), NotNull());
    struct ptree_t* node = yaml_set_value(doc, "test", "value");
    EXPECT_THAT(yaml_get_node(doc, "test"), Eq(node));
    EXPECT_THAT(yaml_get_value(doc, "test"), StrEq("value"));

    yaml_destroy(doc);
}

#if defined(LIGHTSHIP_UTIL_PLATFORM_LINUX) || defined(LIGHTSHIP_UTIL_PLATFORM_MACOSX)
TEST(NAME, load_empty_string)
{
    struct ptree_t* doc;
    ASSERT_THAT((doc = yaml_load_from_memory("  ")), NotNull());
    EXPECT_THAT(map_count(&doc->children), Eq(0));
    yaml_destroy(doc);
}

TEST(NAME, load_bare_minimum_yaml_doc)
{
    struct ptree_t* doc;

    ASSERT_THAT((doc = yaml_load_from_memory(bare_minimum_yml)), NotNull());
    EXPECT_THAT(strcmp(yaml_get_value(doc, "key.key"), "value"), Eq(0));

    yaml_destroy(doc);
}

TEST(NAME, get_value_in_basic_yaml_doc)
{
    struct ptree_t* doc;

    ASSERT_THAT((doc = yaml_load_from_memory(basic_yml)), NotNull());

    EXPECT_THAT(yaml_get_value(doc, "root.players.player1.name"), StrEq("Will Smith"));
    EXPECT_THAT(yaml_get_value(doc, "root.players.player1.age"), StrEq("200"));
    EXPECT_THAT(yaml_get_value(doc, "root.players.player1.sex"), StrEq("female"));
    EXPECT_THAT(yaml_get_value(doc, "root.players.player2.name"), StrEq("TheComet"));
    EXPECT_THAT(yaml_get_value(doc, "root.players.player2.age"), StrEq("21"));
    EXPECT_THAT(yaml_get_value(doc, "root.players.player2.sex"), StrEq("male"));

    EXPECT_THAT(yaml_get_value(doc, "root.enemies.enemy1.name"), StrEq("George Bush"));
    EXPECT_THAT(yaml_get_value(doc, "root.enemies.enemy1.age"), StrEq("9001"));
    EXPECT_THAT(yaml_get_value(doc, "root.enemies.enemy1.sex"), StrEq("Who knows?"));
    EXPECT_THAT(yaml_get_value(doc, "root.enemies.enemy2.name"), StrEq("Big Daddy"));
    EXPECT_THAT(yaml_get_value(doc, "root.enemies.enemy2.age"), StrEq("394"));
    EXPECT_THAT(yaml_get_value(doc, "root.enemies.enemy2.sex"), StrEq("dad"));

    yaml_destroy(doc);
}

TEST(NAME, get_invalid_value_in_basic_yaml_doc)
{
    struct ptree_t* doc;

    ASSERT_THAT((doc = yaml_load_from_memory(basic_yml)), NotNull());

    EXPECT_THAT(yaml_get_value(doc, "some.invalid.key"), IsNull());
    EXPECT_THAT(yaml_get_value(doc, "root.players.invalid"), IsNull());

    yaml_destroy(doc);
}

TEST(NAME, iterate_nodes_in_basic_yaml_doc)
{
    struct ptree_t* doc;

    ASSERT_THAT((doc = yaml_load_from_memory(basic_yml)), NotNull());

    YAML_FOR_EACH(doc, "root.players", hash, node)
    {
        ASSERT_THAT(yaml_get_value(node, "name"), AnyOf(
            StrEq("Will Smith"),
            StrEq("TheComet")));
    }
    YAML_END_FOR_EACH

    YAML_FOR_EACH(doc, "root.enemies", hash, node)
    {
        ASSERT_THAT(yaml_get_value(node, "name"), AnyOf(
            StrEq("George Bush"),
            StrEq("Big Daddy")));
    }
    YAML_END_FOR_EACH

    yaml_destroy(doc);
}

TEST(NAME, anchors_copy_ptree_correctly)
{
    struct ptree_t* doc;

    ASSERT_THAT((doc = yaml_load_from_memory(anchor_yml)), NotNull());

    ASSERT_THAT(yaml_get_value(doc, "some_thing.another_thing.values.item1"), StrEq("value1"));
    ASSERT_THAT(yaml_get_value(doc, "some_thing.another_thing.values.item2"), StrEq("value2"));
    ASSERT_THAT(yaml_get_value(doc, "some_thing.another_thing2.values.item1"), StrEq("value1"));
    ASSERT_THAT(yaml_get_value(doc, "some_thing.another_thing2.values.item2"), StrEq("value2"));

    yaml_destroy(doc);
}

TEST(NAME, dont_copy_anchors_into_nodes_sharing_the_same_names)
{
    ASSERT_THAT(yaml_load_from_memory(anchor_same_names_yml), IsNull());
}

TEST(NAME, lists1)
{
    struct ptree_t* doc;

    ASSERT_THAT((doc = yaml_load_from_memory(lists1_yml)), NotNull());

    ASSERT_THAT(yaml_get_value(doc, "root.items.0.item1"), StrEq("thing1"));
    ASSERT_THAT(yaml_get_value(doc, "root.items.1.item2"), StrEq("thing2"));
    ASSERT_THAT(yaml_get_value(doc, "root.items.2.item3"), StrEq("thing3"));

    yaml_destroy(doc);
}

TEST(NAME, lists2)
{
    struct ptree_t* doc;

    ASSERT_THAT((doc = yaml_load_from_memory(lists2_yml)), NotNull());

    ASSERT_THAT(yaml_get_value(doc, "root.items.0"), StrEq("thing1"));
    ASSERT_THAT(yaml_get_value(doc, "root.items.1"), StrEq("thing2"));
    ASSERT_THAT(yaml_get_value(doc, "root.items.2"), StrEq("thing3"));

    yaml_destroy(doc);
}

TEST(NAME, lists3)
{
    struct ptree_t* doc;

    ASSERT_THAT((doc = yaml_load_from_memory(lists3_yml)), NotNull());

    ASSERT_THAT(yaml_get_value(doc, "root.items.0"), StrEq("thing1"));
    ASSERT_THAT(yaml_get_value(doc, "root.items.1"), StrEq("thing2"));
    ASSERT_THAT(yaml_get_value(doc, "root.items.2"), StrEq("thing3"));

    yaml_destroy(doc);
}

TEST(NAME, anchors_in_lists1)
{
    struct ptree_t* doc;

    ASSERT_THAT((doc = yaml_load_from_memory(anchor_and_list_yml)), NotNull());

    EXPECT_THAT(yaml_get_value(doc, "items.0.key1"), StrEq("value1"));
    EXPECT_THAT(yaml_get_value(doc, "items.0.key2"), StrEq("value2"));
    EXPECT_THAT(yaml_get_value(doc, "items.1.key1"), StrEq("value1"));
    EXPECT_THAT(yaml_get_value(doc, "items.1.key2"), StrEq("value2"));

    yaml_destroy(doc);
}

TEST(NAME, anchors_in_lists2)
{
    struct ptree_t* doc;

    ASSERT_THAT((doc = yaml_load_from_memory(anchor_and_list2_yml)), NotNull());

    EXPECT_THAT(yaml_get_value(doc, "items.0.key1"), StrEq("value1"));
    EXPECT_THAT(yaml_get_value(doc, "items.0.key2"), StrEq("value2"));
    EXPECT_THAT(yaml_get_value(doc, "items.1.key1"), StrEq("value1"));
    EXPECT_THAT(yaml_get_value(doc, "items.1.key2"), StrEq("value2"));

    yaml_destroy(doc);
}
#endif /* #if defined(LIGHTSHIP_UTIL_PLATFORM_LINUX) || defined(LIGHTSHIP_UTIL_PLATFORM_MACOSX) */
