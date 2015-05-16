#include "gmock/gmock.h"
#include "util/memory.h"
#include "util/yaml.h"
#include "framework/game.h"

using testing::Eq;

class UtilGlobalEnvironment : public testing::Environment
{
public:
    virtual ~UtilGlobalEnvironment() {}

    virtual void SetUp()
    {
        testing::FLAGS_gtest_death_test_style = "threadsafe";
        memory_init();
        yaml_init();
        game_init();
    }

    virtual void TearDown()
    {
        game_deinit();
        yaml_deinit();
        EXPECT_THAT(memory_deinit(), Eq(0)) << "Number of memory leaks";
    }
};

const testing::Environment* const memoryManagementEnvironment =
        testing::AddGlobalTestEnvironment(new UtilGlobalEnvironment);
