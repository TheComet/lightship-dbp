#include "gmock/gmock.h"
#include "util/string.h"
#include "util/memory.h"

#define NAME string

TEST(NAME, cat_strings_works)
{
    char* result = cat_strings(4, "this", " is", " a", " test");
    ASSERT_EQ(0, strcmp(result, "this is a test"));
    FREE(result);
}

TEST(NAME, cat_strings_with_empty_strings)
{
    char* result = cat_strings(4, "", "", "", "");
    ASSERT_EQ(0, strcmp(result, ""));
    FREE(result);
}

TEST(NAME, malloc_string_works)
{
    char* result = malloc_string("this is a test");
    ASSERT_EQ(0, strcmp(result, "this is a test"));
    FREE(result);
}

TEST(NAME, is_number)
{
    ASSERT_NE(0, is_number('0'));
    ASSERT_NE(0, is_number('1'));
    ASSERT_NE(0, is_number('2'));
    ASSERT_NE(0, is_number('3'));
    ASSERT_NE(0, is_number('4'));
    ASSERT_NE(0, is_number('5'));
    ASSERT_NE(0, is_number('6'));
    ASSERT_NE(0, is_number('7'));
    ASSERT_NE(0, is_number('8'));
    ASSERT_NE(0, is_number('9'));
}

TEST(NAME, is_not_number)
{
    /* is_number uses a whitelist in the background so this test really is too long */
    ASSERT_EQ(0, is_number('a'));
    ASSERT_EQ(0, is_number('b'));
    ASSERT_EQ(0, is_number('c'));
    ASSERT_EQ(0, is_number('d'));
    ASSERT_EQ(0, is_number('e'));
    ASSERT_EQ(0, is_number('f'));
    ASSERT_EQ(0, is_number('g'));
    ASSERT_EQ(0, is_number('h'));
    ASSERT_EQ(0, is_number('i'));
    ASSERT_EQ(0, is_number('j'));
    ASSERT_EQ(0, is_number('k'));
    ASSERT_EQ(0, is_number('l'));
    ASSERT_EQ(0, is_number('m'));
    ASSERT_EQ(0, is_number('n'));
    ASSERT_EQ(0, is_number('o'));
    ASSERT_EQ(0, is_number('p'));
    ASSERT_EQ(0, is_number('q'));
    ASSERT_EQ(0, is_number('r'));
    ASSERT_EQ(0, is_number('s'));
    ASSERT_EQ(0, is_number('t'));
    ASSERT_EQ(0, is_number('u'));
    ASSERT_EQ(0, is_number('v'));
    ASSERT_EQ(0, is_number('w'));
    ASSERT_EQ(0, is_number('x'));
    ASSERT_EQ(0, is_number('y'));
    ASSERT_EQ(0, is_number('z'));
}
