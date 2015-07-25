#include "gmock/gmock.h"
#include "util/memory.h"
#include "util/bst_vector.h"

#define NAME bstv_malloc

using namespace testing;

TEST(NAME, create)
{
    force_malloc_fail_on();
    EXPECT_THAT(bstv_create(), IsNull());
    force_malloc_fail_off();

    struct bstv_t* map;
    ASSERT_THAT((map = bstv_create()), NotNull());
    bstv_destroy(map);
}

TEST(NAME, insert)
{
    struct bstv_t* map = bstv_create();

    force_malloc_fail_on();
    EXPECT_THAT(bstv_insert(map, 1, NULL), Eq(0));
    force_malloc_fail_off();

    EXPECT_THAT(bstv_insert(map, 1, NULL), Ne(0));

    bstv_destroy(map);
}
