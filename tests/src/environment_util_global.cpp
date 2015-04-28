#include "gmock/gmock.h"
#include "util/memory.h"
#include "util/yaml.h"

class UtilGlobalEnvironment : public testing::Environment
{
public:
    virtual ~UtilGlobalEnvironment() {}

    virtual void SetUp()
    {
        memory_init();
        yaml_init();
    }

    virtual void TearDown()
    {
        yaml_deinit();
        memory_deinit();
    }
};

const testing::Environment* const memoryManagementEnvironment =
        testing::AddGlobalTestEnvironment(new UtilGlobalEnvironment);
