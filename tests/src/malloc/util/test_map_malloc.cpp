#include "gmock/gmock.h"
#include "util/map.h"
#include "util/memory.h"

#define NAME map_malloc

using namespace testing;

TEST(NAME, create)
{
    force_malloc_fail_on();
    ASSERT_THAT(map_create(), IsNull());
    force_malloc_fail_off();
}
