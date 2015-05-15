#include "gmock/gmock.h"
#include "util/memory.h"

#define NAME memory_malloc

using namespace testing;

TEST(NAME, malloc)
{
    void* p1 = MALLOC(1);
    ASSERT_THAT(p1, NotNull());
    FREE(p1);

    force_malloc_fail_on();
    void* p2 = MALLOC(1);
    ASSERT_THAT(p2, IsNull());
    force_malloc_fail_off();

    void* p3 = MALLOC(1);
    ASSERT_THAT(p1, NotNull());
    FREE(p3);
}
