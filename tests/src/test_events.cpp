#include "gmock/gmock.h"
#include "util/events.h"

#define NAME events

struct NAME : public testing::Test
{
    virtual void SetUp()
    {
        events_init();
    }
    
    virtual void TearDown()
    {
        events_deinit();
    }
};

TEST_F(NAME, create_event)
{
    
}
