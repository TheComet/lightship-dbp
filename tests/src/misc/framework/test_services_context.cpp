#include "gmock/gmock.h"
#include "framework/game.h"
#include "framework/plugin.h"
#include "framework/services.h"

#define NAME services_context

using namespace testing;

class NAME : public Test
{
public:

    virtual void SetUp()
    {
        game = game_create("test", GAME_CLIENT);
        plugin = plugin_create(game, "test", "test", "test", "test", "test");
    }

    virtual void TearDown()
    {
        plugin_destroy(plugin);
        game_destroy(game);
    }

    struct game_t* game;
    struct plugin_t* plugin;
};

int callback1(unsigned int a, double b, char* str)
{
    return a + b + strlen(str);
}

TEST_F(NAME, create_verify_type_info)
{
    struct service_t* s;
    SERVICE_CREATE3(plugin, s, "test.service", (service_func)callback1, int, unsigned int, double, char*);

    ASSERT_THAT(s, NotNull());

    // general structure
    EXPECT_THAT(s->directory, StrEq("test.service"));
    EXPECT_THAT(s->exec, Eq((service_func)callback1));
    EXPECT_THAT(s->plugin, Eq(plugin));
    EXPECT_THAT(s->plugin->game, Eq(game));

    // check type info
    EXPECT_THAT(s->type_info.argc, Eq(3));
    EXPECT_THAT(s->type_info.argv_type[0], Eq(SERVICE_TYPE_UINT32));
    EXPECT_THAT(s->type_info.argv_type[1], Eq(SERVICE_TYPE_DOUBLE));
    EXPECT_THAT(s->type_info.argv_type[2], Eq(SERVICE_TYPE_STRING));
    EXPECT_THAT(s->type_info.argv_type_str[0], StrEq("unsigned int"));
    EXPECT_THAT(s->type_info.argv_type_str[1], StrEq("double"));
    EXPECT_THAT(s->type_info.argv_type_str[2], StrEq("char*"));
    EXPECT_THAT(s->type_info.ret_type, Eq(SERVICE_TYPE_INT32));
    EXPECT_THAT(s->type_info.ret_type_str, StrEq("int"));

    service_destroy(s);
}
