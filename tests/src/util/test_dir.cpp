#include "gmock/gmock.h"
#include "util/dir.h"
#include "util/linked_list.h"
#include "util/string.h"

#define NAME dir

char
list_find(struct list_t* list, const char* name)
{
    LIST_FOR_EACH(list, char, search_name)
    {
        if(strcmp(name, search_name) == 0)
            return 1;
    }
    
    return 0;
}

TEST(NAME, empty_directory)
{
    struct list_t* list = list_create();
    
#define SEARCH_DIR_EMPTY "tests/test_dir/empty/"
    ASSERT_NE(0, get_directory_listing(list, SEARCH_DIR_EMPTY));
    ASSERT_EQ(2, list->count); /* current and parent directory */
    ASSERT_NE(0, list_find(list, SEARCH_DIR_EMPTY "."));
    ASSERT_NE(0, list_find(list, SEARCH_DIR_EMPTY ".."));
    
    LIST_FOR_EACH(list, char, file)
        free_string(file);
    
    list_destroy(list);
}

TEST(NAME, files_and_directories)
{
    int i = 0;
    struct list_t* list = list_create();
    
#define SEARCH_DIR_FILES "tests/test_dir/files/"
    ASSERT_NE(0, get_directory_listing(list, "tests/test_dir/files/"));
    ASSERT_EQ(7, list->count);
    ASSERT_NE(0, list_find(list, SEARCH_DIR_FILES "."));
    ASSERT_NE(0, list_find(list, SEARCH_DIR_FILES ".."));
    ASSERT_NE(0, list_find(list, SEARCH_DIR_FILES "file_a.txt"));
    ASSERT_NE(0, list_find(list, SEARCH_DIR_FILES "file_b.txt"));
    ASSERT_NE(0, list_find(list, SEARCH_DIR_FILES "file_c.txt"));
    ASSERT_NE(0, list_find(list, SEARCH_DIR_FILES "dir_a"));
    ASSERT_NE(0, list_find(list, SEARCH_DIR_FILES "dir_b"));
    
    LIST_FOR_EACH(list, char, file)
        free_string(file);
    
    list_destroy(list);
}

TEST(NAME, invalid_directory)
{
    struct list_t* list = list_create();
    
    ASSERT_EQ(0, get_directory_listing(list, "tests/test_dir/invalid/"));
    ASSERT_EQ(0, list->count);
    
    list_destroy(list);
}