#include "gmock/gmock.h"
#include "framework/events.h"
#include "framework/services.h"
#include "framework/plugin.h"
#include "framework/game.h"
#include "framework/log.h"

#define NAME event

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

TEST_F(NAME, create_event_inits_correctly)
{
    struct event_t* event;
    EVENT_CREATE0(plugin, event, "test.event");

    ASSERT_THAT(strcmp("test.event", event->directory), Eq(0));
    ASSERT_THAT(event->listeners.capacity, Eq(0));
    ASSERT_THAT(event->listeners.count, Eq(0));
    ASSERT_THAT(event->listeners.element_size, Eq(sizeof(struct event_listener_t)));
    ASSERT_THAT(event->listeners.data, IsNull());

    event_destroy(event);
}
