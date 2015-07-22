#include "gmock/gmock.h"
#include "util/dir.h"
#include "util/linked_list.h"
#include "util/string.h"

#define NAME dir

using testing::Eq;
using testing::Ne;

char
list_find(struct list_t* list, const char* name)
{
    LIST_FOR_EACH(list, char, search_name)
        if(strcmp(name, search_name) == 0)
            return 1;
    LIST_END_EACH

    return 0;
}

TEST(NAME, empty_directory)
{
    struct list_t* list = list_create();

#define SEARCH_DIR_EMPTY "tests/test_dir/empty/"
    EXPECT_THAT(get_directory_listing(list, SEARCH_DIR_EMPTY), Ne(0));
    EXPECT_THAT(list->count, Eq(2)); /* current and parent directory */
    EXPECT_THAT(list_find(list, SEARCH_DIR_EMPTY "."), Ne(0));
    EXPECT_THAT(list_find(list, SEARCH_DIR_EMPTY ".."), Ne(0));

    LIST_FOR_EACH(list, char, file)
        free_string(file);
	LIST_END_EACH

    list_destroy(list);
}

TEST(NAME, files_and_directories)
{
    int i = 0;
    struct list_t* list = list_create();

#define SEARCH_DIR_FILES "tests/test_dir/files/"
    EXPECT_THAT(get_directory_listing(list, SEARCH_DIR_FILES), Ne(0));
    EXPECT_THAT(list->count, Eq(7));
    EXPECT_THAT(list_find(list, SEARCH_DIR_FILES "."), Ne(0));
    EXPECT_THAT(list_find(list, SEARCH_DIR_FILES ".."), Ne(0));
    EXPECT_THAT(list_find(list, SEARCH_DIR_FILES "file_a.txt"), Ne(0));
    EXPECT_THAT(list_find(list, SEARCH_DIR_FILES "file_b.txt"), Ne(0));
    EXPECT_THAT(list_find(list, SEARCH_DIR_FILES "file_c.txt"), Ne(0));
    EXPECT_THAT(list_find(list, SEARCH_DIR_FILES "dir_a"), Ne(0));
    EXPECT_THAT(list_find(list, SEARCH_DIR_FILES "dir_b"), Ne(0));

    LIST_FOR_EACH(list, char, file)
        free_string(file);
	LIST_END_EACH

    list_destroy(list);
}

TEST(NAME, invalid_directory)
{
    struct list_t* list = list_create();

    EXPECT_THAT(get_directory_listing(list, "tests/test_dir/invalid/"), Eq(0));
    EXPECT_THAT(list->count, Eq(0));

    list_destroy(list);
}
