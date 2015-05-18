#include "gmock/gmock.h"
#include "framework/game.h"
#include "framework/plugin.h"
#include "framework/services.h"

#define NAME service

using namespace testing;

SERVICE(callback1)
{
    EXTRACT_ARGUMENT(0, a, int, int);
    EXTRACT_ARGUMENT(1, b, double, double);
    EXTRACT_ARGUMENT_PTR(2, str, const char*);
    RETURN(a + b + strlen(str), int);
}

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
    EXPECT_THAT(s->type_info->argc, Eq(3));
    EXPECT_THAT(s->type_info->argv_type[0], Eq(TYPE_UINT32));
    EXPECT_THAT(s->type_info->argv_type[1], Eq(TYPE_DOUBLE));
    EXPECT_THAT(s->type_info->argv_type[2], Eq(TYPE_STRING));
    EXPECT_THAT(s->type_info->argv_type_str[0], StrEq("unsigned int"));
    EXPECT_THAT(s->type_info->argv_type_str[1], StrEq("double"));
    EXPECT_THAT(s->type_info->argv_type_str[2], StrEq("char*"));
    EXPECT_THAT(s->type_info->ret_type, Eq(TYPE_INT32));
    EXPECT_THAT(s->type_info->ret_type_str, StrEq("int"));

    service_destroy(s);
}

TEST_F(NAME, verify_registration_in_game_service_directory_and_in_plugin)
{
    struct service_t* s;
    SERVICE_CREATE3(plugin, s, "test.service", (service_func)callback1, int, unsigned int, double, char*);

    ASSERT_THAT(s, NotNull());

    // check that it exists in the game's service directory
    struct ptree_t* service_node = ptree_get_node(&game->services, "test.service");
    EXPECT_THAT(service_node, NotNull());
    if(service_node) EXPECT_THAT(service_node->value, Eq(s));

    // check that the plugin object has a reference to it
    struct service_t** servp = (struct service_t**)unordered_vector_get_element(&plugin->services, 0);
    EXPECT_THAT(servp, NotNull());
    if(servp) EXPECT_THAT(*servp, Eq(s));

    service_destroy(s);

    // check that it now no longer exists in the game's service directory
    EXPECT_THAT(ptree_get_node(&game->services, "test.service"), IsNull());

    // same for plugin
    EXPECT_THAT(unordered_vector_get_element(&plugin->services, 0), IsNull());
}
