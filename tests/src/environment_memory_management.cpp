#include "gmock/gmock.h"
#include "util/memory.h"

class MemoryManagementEnvironment : public testing::Environment
{
public:
    virtual ~MemoryManagementEnvironment() {}

    virtual void SetUp()
    {
        memory_init();
    }

    virtual void TearDown()
    {
        memory_deinit();
    }
};

testing::Environment* const memoryManagementEnvironment = testing::AddGlobalTestEnvironment(new MemoryManagementEnvironment);
