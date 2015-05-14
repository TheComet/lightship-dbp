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

TEST(NAME, insert)
{
    struct map_t* map = map_create();

    force_malloc_fail_on();
    ASSERT_THAT(map_insert(map, 1, NULL), Eq(0));
    force_malloc_fail_off();

    map_destroy(map);
}
