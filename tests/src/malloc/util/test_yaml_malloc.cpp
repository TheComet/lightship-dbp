#include "gmock/gmock.h"
#include "util/yaml.h"
#include "util/ptree.h"
#include "util/memory.h"

#define NAME yaml_malloc

using namespace testing;

static const char* yml =
"item:\n"
"    key1: value1\n"
"    key2: value2\n"
"items:\n"
"  - *item\n"
"  - *item\n";

TEST(NAME, create)
{
    force_malloc_fail_on();
    EXPECT_THAT(yaml_create(), IsNull());
    force_malloc_fail_off();

    struct ptree_t* doc = yaml_create();
    ASSERT_THAT(doc, NotNull());
    yaml_destroy(doc);
}

#if defined(LIGHTSHIP_UTIL_PLATFORM_LINUX) || defined(LIGHTSHIP_UTIL_PLATFORM_MACOSX)
TEST(NAME, load)
{
#ifdef _DEBUG
    for(int i = 1; i != 50; ++i)
#else
    for(int i = 1; i != 25; ++i)
#endif
    {
        force_malloc_fail_after(i);
        EXPECT_THAT(yaml_load_from_memory(yml), IsNull());
        force_malloc_fail_off();
    }

    struct ptree_t* doc = yaml_load_from_memory(yml);
    ASSERT_THAT(doc, NotNull());
    yaml_destroy(doc);
}
#endif /* #if defined(LIGHTSHIP_UTIL_PLATFORM_LINUX) || defined(LIGHTSHIP_UTIL_PLATFORM_MACOSX) */

TEST(NAME, set_value)
{
    struct ptree_t* doc = yaml_create();
    yaml_set_value(doc, "key1.key2", "value");
#ifdef _DEBUG
    for(int i = 1; i != 8; ++i)
#else
    for(int i = 1; i != 4; ++i)
#endif
    {
        force_malloc_fail_after(i);
        EXPECT_THAT(yaml_set_value(doc, "test.whatever", "value"), IsNull());
        force_malloc_fail_off();
    }

    yaml_destroy(doc);
}
