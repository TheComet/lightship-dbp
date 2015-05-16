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

int callback1(int a, double b, char* str)
{
    return a + b + strlen(str);
}

TEST_F(NAME, create_verify_type_info)
{
    struct service_t* s;
    SERVICE_CREATE3(plugin, s, "test.service", (service_func)callback1, int, int, double, char*);
    service_destroy(s);
}
