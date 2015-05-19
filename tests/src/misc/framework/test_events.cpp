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
    struct event_t* event = event_create(plugin, "test.event");

    ASSERT_THAT(strcmp("test.event", event->directory), Eq(0));
    ASSERT_THAT(event->listeners.capacity, Eq(0));
    ASSERT_THAT(event->listeners.count, Eq(0));
    ASSERT_THAT(event->listeners.element_size, Eq(sizeof(struct event_listener_t)));
    ASSERT_THAT(event->listeners.data, IsNull());

    event_destroy(event);
}

static char g_listener_triggered_1 = 0;
static char g_listener_triggered_2 = 0;
EVENT_C0(evt_1);
EVENT_C0(evt_2);
EVENT_LISTENER0(listener1) { g_listener_triggered_1 = 1; }
EVENT_LISTENER0(listener2) { g_listener_triggered_2 = 1; }

TEST_F(NAME, listeners_can_be_registered_froplugin_obj)
{
    struct event_t* event = event_create(plugin, "test.event");

    event_register_listener(game, "test.event", (event_callback_func)listener1);
    ASSERT_EQ(1, event->listeners.count);
    event_register_listener(game, "test.event", (event_callback_func)listener2);
    ASSERT_EQ(2, event->listeners.count);

    struct event_listener_t* listeners = (struct event_listener_t*)event->listeners.data;

    event_destroy(event);
}

TEST_F(NAME, listeners_receive_events_when_fired)
{
    evt_1 = event_create(plugin, "event");
    event_register_listener(game, "test.event", (event_callback_func)listener1);
    event_register_listener(game, "test.event", (event_callback_func)listener2);

    g_listener_triggered_1 = 0; g_listener_triggered_2 = 0;
    EVENT_FIRE0(evt_1);

    ASSERT_EQ(1, g_listener_triggered_1);
    ASSERT_EQ(1, g_listener_triggered_2);

    event_destroy(evt_1);
}
