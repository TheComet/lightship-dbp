#include "gmock/gmock.h"
#include "util/string.h"
#include "util/memory.h"

#define NAME string_malloc

using namespace testing;

TEST(NAME, cat_strings)
{
    force_malloc_fail_on();
    EXPECT_THAT(cat_strings(2, "hello ", "world"), IsNull());
    force_malloc_fail_off();

    char* result = cat_strings(2, "hello ", "world");
    ASSERT_THAT(result, StrEq("hello world"));
    free_string(result);
}

TEST(NAME, malloc_string)
{
    force_malloc_fail_on();
    EXPECT_THAT(malloc_string("test"), IsNull());
    force_malloc_fail_off();

    char* result = malloc_string("test");
    ASSERT_THAT(result, StrEq("test"));
    free_string(result);
}

TEST(NAME, cat_wstrings)
{
    force_malloc_fail_on();
    EXPECT_THAT(cat_wstrings(2, L"hello ", L"world"), IsNull());
    force_malloc_fail_off();

    wchar_t* result = cat_wstrings(2, L"hello ", L"world");
    ASSERT_THAT(result, StrEq(L"hello world"));
    free_string(result);
}

TEST(NAME, malloc_wstring)
{
    force_malloc_fail_on();
    EXPECT_THAT(malloc_wstring(L"test"), IsNull());
    force_malloc_fail_off();

    wchar_t* result = malloc_wstring(L"test");
    ASSERT_THAT(result, StrEq(L"test"));
    free_string(result);
}

TEST(NAME, strtowcs)
{
    force_malloc_fail_on();
    EXPECT_THAT(strtowcs("test"), IsNull());
    force_malloc_fail_off();

    wchar_t* result = strtowcs("test");
    ASSERT_THAT(result, StrEq(L"test"));
    free_string(result);
}

TEST(NAME, wcstostr)
{
    force_malloc_fail_on();
    EXPECT_THAT(wcstostr(L"test"), IsNull());
    force_malloc_fail_off();

    char* result = wcstostr(L"test");
    ASSERT_THAT(result, StrEq("test"));
    free_string(result);
}
