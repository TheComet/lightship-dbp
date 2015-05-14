#include "gmock/gmock.h"
#include "util/dir.h"
#include "util/memory.h"
#include "util/linked_list.h"

#define NAME dir_malloc

using namespace testing;

TEST(NAME, get_directory_listing_invalid_path)
{
    struct list_t* list = list_create();

    force_malloc_fail_on();
    ASSERT_THAT(get_directory_listing(list, "some/directory/no/one/cares"), Eq(0));
    force_malloc_fail_off();

    ASSERT_THAT(list->count, Eq(0));

    list_destroy(list);
}

TEST(NAME, get_directory_listing_valid_path)
{
    struct list_t* list = list_create();

#define SEARCH_DIR_FILES "tests/test_dir/files/"
    force_malloc_fail_on();
    ASSERT_THAT(get_directory_listing(list, SEARCH_DIR_FILES), Eq(0));
    force_malloc_fail_off();

    ASSERT_THAT(list->count, Eq(0));

    list_destroy(list);
}