#include "gmock/gmock.h"
#include "util/yaml.h"
#include "util/memory.h"

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

TEST(NAME, get_value_on_basic_yaml)
{
    struct yaml_doc_t* doc;
    ASSERT_THAT(1, Eq(0)); /* yaml currently broken, mark this test as not implemented */

    ASSERT_THAT((doc = yaml_load_from_memory(basic_yml)), NotNull());

    EXPECT_THAT(yaml_get_value(doc, "root.players.player.name"), AnyOf(
        StrEq("Will Smith"),
        StrEq("TheComet")));
    EXPECT_THAT(yaml_get_value(doc, "root.enemies.enemy.name"), AnyOf(
        StrEq("George Bush"),
        StrEq("Big Daddy")));

    yaml_destroy(doc);
}
