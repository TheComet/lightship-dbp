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

/*
 * Test listener for testing.
 * Arguments are (int32_t, float, const char*).
 *
 * Will test if a == (int32_t)b;
 * Will test if c contains the string "test argument".
 */
EVENT_LISTENER(test_listener)
{
	EXTRACT_ARGUMENT(0, a, int32_t, int32_t);
	EXTRACT_ARGUMENT(1, b, float, float);
	EXTRACT_ARGUMENT_PTR(2, c, const char*);

	EXPECT_THAT(a, Eq((int32_t)b));
	EXPECT_THAT(c, StrEq("test argument"));
}

TEST_F(NAME, create_event_inits_correctly)
{
	struct event_t* event;
    EVENT_CREATE3(plugin, event, "test.event", int32_t, float, const char*);

	ASSERT_THAT(event, NotNull());
    EXPECT_THAT(event->directory, StrEq("test.event"));
    EXPECT_THAT(event->listeners.capacity, Eq(0));
    EXPECT_THAT(event->listeners.count, Eq(0));
    EXPECT_THAT(event->listeners.element_size, Eq(sizeof(struct event_listener_t)));
    EXPECT_THAT(event->listeners.data, IsNull());

	ASSERT_THAT(event->type_info, NotNull());
	EXPECT_THAT(event->type_info->argc, Eq(3));
	EXPECT_THAT(event->type_info->argv_type[0], Eq(TYPE_INT32));
	EXPECT_THAT(event->type_info->argv_type[1], Eq(TYPE_FLOAT));
	EXPECT_THAT(event->type_info->argv_type[2], Eq(TYPE_STRING));
	EXPECT_THAT(event->type_info->has_unknown_types, Eq(0));
	EXPECT_THAT(event->type_info->ret_type, Eq(TYPE_VOID));
}

TEST_F(NAME, create_and_destroy_event)
{
	struct event_t* event;
	EVENT_CREATE3(plugin, event, "test.event1", uint32_t, float, const char*);
	EVENT_CREATE0(plugin, event, "test.event2");

	struct event_t* event1 = event_get(game, "test.event1");
	struct event_t* event2 = event_get(game, "test.event2");
	ASSERT_THAT(event1, NotNull());
	ASSERT_THAT(event2, NotNull());

	event_destroy(event1);

	EXPECT_THAT(event_get(game, "test.event1"), IsNull());
	EXPECT_THAT(event_get(game, "test.event2"), NotNull());

	event_destroy(event2);

	EXPECT_THAT(event_get(game, "test.event1"), IsNull());
	EXPECT_THAT(event_get(game, "test.event2"), IsNull());
}

TEST_F(NAME, event_get)
{
	struct event_t* event;
	EVENT_CREATE0(plugin, event, "test.event");
	EVENT_CREATE0(plugin, event, "test.another.event");

	EXPECT_THAT(event_get(game, "test.event"), NotNull());
	EXPECT_THAT(event_get(game, "test.another.event"), NotNull());
	EXPECT_THAT(event_get(game, "test.nothing"), IsNull());

	EXPECT_THAT(event_get(game, ""), IsNull());
	EXPECT_THAT(event_get(game, "...."), IsNull());
}

TEST_F(NAME, event_register_and_unregister_listeners)
{
	struct event_t* event;
	EVENT_CREATE3(plugin, event, "test.event", int, float, const char*);

	EXPECT_THAT(event_register_listener(game, "test.not.an.event", test_listener), Eq(0));
	EXPECT_THAT(event_register_listener(game, "test.event", test_listener), Ne(0));
	EXPECT_THAT(event_register_listener(game, "test.event", test_listener), Eq(0));
}
