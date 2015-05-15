#include "gmock/gmock.h"
#include "util/dir.h"
#include "util/memory.h"
#include "util/linked_list.h"
#include "util/string.h"

#define NAME dir_malloc

using namespace testing;

TEST(NAME, get_directory_listing_invalid_path)
{
    struct list_t* list = list_create();

    force_malloc_fail_on();
    EXPECT_THAT(get_directory_listing(list, "some/directory/no/one/cares"), Eq(0));
    force_malloc_fail_off();

    EXPECT_THAT(list->count, Eq(0));

    list_destroy(list);
}

TEST(NAME, get_directory_listing_valid_path)
{
    struct list_t* list = list_create();

#define DIR "tests/test_dir/files/"
    force_malloc_fail_on();
    EXPECT_THAT(get_directory_listing(list, DIR), Eq(0));
    force_malloc_fail_off();
    EXPECT_THAT(list->count, Eq(0));

    EXPECT_THAT(get_directory_listing(list, DIR), Ne(0));
    ASSERT_THAT(list->count, Eq(7));
    LIST_FOR_EACH(list, char, file)
        free_string(file);

    list_destroy(list);
}
