#include "gmock/gmock.h"
#include "util/yaml.h"
#include "util/ptree.h"

#define NAME yaml

using namespace testing;

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
