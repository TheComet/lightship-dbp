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
/*
TEST(NAME, load_fail_immediately)
{
    force_malloc_fail_on();
    EXPECT_THAT(yaml_load_from_memory(yml), IsNull());
    force_malloc_fail_off();

    struct ptree_t* tree = yaml_load_from_memory(yml);
    ASSERT_THAT(tree, NotNull());
    yaml_destroy(tree);
}

TEST(NAME, load_fail_later)
{
    for(int i = 2; i != 5; ++i)
    {
        force_malloc_fail_after(i);
        EXPECT_THAT(yaml_load_from_memory(yml), IsNull());
        force_malloc_fail_off();
    }
}*/
