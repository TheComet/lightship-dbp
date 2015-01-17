#include "gmock/gmock.h"
#include "util/events.h"
#include "util/api.h"

#define NAME events

struct NAME : public testing::Test
{
    NAME()
    {
        m_plugin.handle = NULL;
        m_plugin.info.author = const_cast<char*>("Test");
        m_plugin.info.category = const_cast<char*>("Testing");
        m_plugin.info.description = const_cast<char*>("A test");
        m_plugin.info.website = const_cast<char*>("plugin.org");
        m_plugin.info.language = PLUGIN_PROGRAMMING_LANGUAGE_CPP;
        m_plugin.info.name = const_cast<char*>("test");
        m_plugin.info.version.major = 0;
        m_plugin.info.version.minor = 0;
        m_plugin.info.version.patch = 1;
        m_plugin.init = NULL;
        m_plugin.start = NULL;
        m_plugin.stop = NULL;
        
        plugin = &m_plugin;
    }

    virtual void SetUp()
    {
        services_init();
        events_init();
    }
    
    virtual void TearDown()
    {
        events_deinit();
        services_deinit();
    }
    struct plugin_t* plugin;
    struct plugin_t m_plugin;
};

TEST_F(NAME, create_event_inits_correctly)
{
    struct event_t* event = event_create(plugin, "event");
    ASSERT_EQ(0, strcmp("test.event", event->name));
    ASSERT_EQ(0, event->listeners.capacity);
    ASSERT_EQ(0, event->listeners.count);
    ASSERT_EQ(sizeof(struct event_listener_t), event->listeners.element_size);
    ASSERT_EQ((void*)0, event->listeners.data);
    ASSERT_EQ(0, event_destroy(NULL));
    ASSERT_EQ(1, event_destroy(event));
}

static char g_listener_triggered_1 = 0;
static char g_listener_triggered_2 = 0;
EVENT_LISTENER0(listener1) { g_listener_triggered_1 = 1; }
EVENT_LISTENER0(listener2) { g_listener_triggered_2 = 1; }

TEST_F(NAME, listeners_can_be_registered_from_plugin)
{
    struct event_t* event = event_create(plugin, "event");
    
    event_register_listener(plugin, "test.event", (event_callback_func)listener1);
    ASSERT_EQ(1, event->listeners.count);
    event_register_listener(plugin, "test.event", (event_callback_func)listener2);
    ASSERT_EQ(2, event->listeners.count);

    event_destroy(event);
}

TEST_F(NAME, listeners_receive_events_when_fired)
{
    struct event_t* event = event_create(plugin, "event");
    event_register_listener(plugin, "test.event", (event_callback_func)listener1);
    event_register_listener(plugin, "test.event", (event_callback_func)listener2);

    g_listener_triggered_1 = 0; g_listener_triggered_2 = 0;
    EVENT_FIRE0(event);
    
    ASSERT_EQ(1, g_listener_triggered_1);
    ASSERT_EQ(1, g_listener_triggered_2);
    
    event_destroy(event);
}

