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

TEST_F(NAME, create_event_and_destroy)
{
    struct event_t* event = event_create(plugin, "event");
    ASSERT_EQ(0, strcmp("test.event", event->name));
    event_destroy(event);
}
