#include "tests/globals.hpp"
#include "gmock/gmock.h"
#include "util/dir.h"
#include "util/linked_list.h"

#define NAME dir_DeathTest

using namespace testing;

TEST(NAME, get_directory_listing_with_null_list)
{
    ASSERT_DEATH(get_directory_listing(NULL, "tests/test_dir/files"), ASSERTION_REGEX);
}

TEST(NAME, get_directory_listing_with_null_path)
{
    struct list_t* list = list_create();
    EXPECT_DEATH(get_directory_listing(list, NULL), ASSERTION_REGEX);
    list_destroy(list);
}
