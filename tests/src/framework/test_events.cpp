#include "gmock/gmock.h"
#include "framework/events.h"
#include "framework/services.h"
#include "framework/plugin_api.h"
#include "framework/game.h"

#define NAME events

struct NAME : public testing::Test
{
    NAME()
    {
        plugin_obj.handle = NULL;
        plugin_obj.info.author = const_cast<char*>("Test");
        plugin_obj.info.category = const_cast<char*>("Testing");
        plugin_obj.info.description = const_cast<char*>("A test");
        plugin_obj.info.website = const_cast<char*>("www.plugin.test");
        plugin_obj.info.language = PLUGIN_PROGRAMMING_LANGUAGE_CPP;
        plugin_obj.info.name = const_cast<char*>("test");
        plugin_obj.info.version.major = 0;
        plugin_obj.info.version.minor = 0;
        plugin_obj.info.version.patch = 1;
        plugin_obj.init = NULL;
        plugin_obj.start = NULL;
        plugin_obj.stop = NULL;

        plugin = &plugin_obj;
    }

    virtual void SetUp()
    {
        game = game_create("test_game", GAME_HOST);
    }

    virtual void TearDown()
    {
        game_destroy(game);
    }

    struct game_t* game;
    struct plugin_t* plugin;
private:
    struct plugin_t plugin_obj;
};

TEST_F(NAME, create_event_inits_correctly)
{
    struct event_t* event = event_create(game, "test.event");

    ASSERT_EQ(0, strcmp("test.event", event->directory));
    ASSERT_EQ(0, event->listeners.capacity);
    ASSERT_EQ(0, event->listeners.count);
    ASSERT_EQ(sizeof(struct event_listener_t), event->listeners.element_size);
    ASSERT_EQ((void*)0, event->listeners.data);
    ASSERT_EQ(0, event_destroy(NULL));

    ASSERT_EQ(1, event_destroy(event));
}

static char g_listener_triggered_1 = 0;
static char g_listener_triggered_2 = 0;
EVENT_C0(evt_1);
EVENT_C0(evt_2);
EVENT_LISTENER0(listener1) { g_listener_triggered_1 = 1; }
EVENT_LISTENER0(listener2) { g_listener_triggered_2 = 1; }

TEST_F(NAME, listeners_can_be_registered_froplugin_obj)
{
    struct event_t* event = event_create(plugin, "event");

    event_register_listener(plugin, "test.event", (event_callback_func)listener1);
    ASSERT_EQ(1, event->listeners.count);
    event_register_listener(plugin, "test.event", (event_callback_func)listener2);
    ASSERT_EQ(2, event->listeners.count);

    struct event_listener_t* listeners = (struct event_listener_t*)event->listeners.data;
    ASSERT_EQ(0, strcmp("test.", listeners[0].name_space));
    ASSERT_EQ((void*)listener1, (void*)listeners[0].exec);
    ASSERT_EQ(0, strcmp("test.", listeners[1].name_space));
    ASSERT_EQ((void*)listener2, (void*)listeners[1].exec);

    event_destroy(event);
}

TEST_F(NAME, listeners_receive_events_when_fired)
{
    evt_1 = event_create(plugin, "event");
    event_register_listener(plugin, "test.event", (event_callback_func)listener1);
    event_register_listener(plugin, "test.event", (event_callback_func)listener2);

    g_listener_triggered_1 = 0; g_listener_triggered_2 = 0;
    EVENT_FIRE0(evt_1);

    ASSERT_EQ(1, g_listener_triggered_1);
    ASSERT_EQ(1, g_listener_triggered_2);

    event_destroy(evt_1);
}
