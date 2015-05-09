#include "gmock/gmock.h"
#include "util/yaml.h"
#include "util/ptree.h"

#define NAME yaml

using namespace testing;

static const char* basic_yml =
"root:\n"
"    players:\n"
"        player:\n"
"            name: Will Smith\n"
"            age: 200\n"
"            sex: female\n"
"        player:\n"
"            name: TheComet\n"
"            age: 21\n"
"            sex: male\n"
"    enemies:\n"
"        enemy:\n"
"            name: George Bush\n"
"            age: 9001\n"
"            sex: Who knows?\n"
"        enemy:\n"
"            name: Big Daddy\n"
"            age: 394\n"
"            sex: dad\n";

TEST(NAME, get_value_in_basic_yaml_doc)
{
    struct yaml_doc_t* doc;

    ASSERT_THAT((doc = yaml_load_from_memory(basic_yml)), NotNull());

    EXPECT_THAT(yaml_doc_get_value(doc, "root.players.player.name"), AnyOf(
        StrEq("Will Smith"),
        StrEq("TheComet")));
    EXPECT_THAT(yaml_doc_get_value(doc, "root.enemies.enemy.name"), AnyOf(
        StrEq("George Bush"),
        StrEq("Big Daddy")));

    yaml_destroy(doc);
}

TEST(NAME, get_invalid_value_in_basic_yaml_doc)
{
    struct yaml_doc_t* doc;

    ASSERT_THAT((doc = yaml_load_from_memory(basic_yml)), NotNull());

    EXPECT_THAT(yaml_doc_get_value(doc, "some.invalid.key"), IsNull());
    EXPECT_THAT(yaml_doc_get_value(doc, "root.players.invalid"), IsNull());

    yaml_destroy(doc);
}

TEST(NAME, iterate_nodes_in_basic_yaml_doc)
{
    struct yaml_doc_t* doc;

    ASSERT_THAT((doc = yaml_load_from_memory(basic_yml)), NotNull());

    YAML_FOR_EACH(doc, "root.players", hash, node)
    {
        ASSERT_THAT(yaml_node_get_value(node, "player.name"), AnyOf(
            StrEq("Will Smith"),
            StrEq("TheComet")));
    }
    YAML_END_FOR_EACH

    YAML_FOR_EACH(doc, "root.enemies", hash, node)
    {
        ASSERT_THAT(yaml_node_get_value(node, "enemy.name"), AnyOf(
            StrEq("George Bush"),
            StrEq("Big Daddy")));
    }
    YAML_END_FOR_EACH

    yaml_destroy(doc);
}
