#include "gmock/gmock.h"
#include "util/string.h"
#include "util/memory.h"

#define NAME string

using namespace testing;

TEST(NAME, cat_strings)
{
    char* result = cat_strings(4, "this", " is", " a", " test");
    EXPECT_THAT(strcmp(result, "this is a test"), Eq(0));
    free_string(result);
}

TEST(NAME, cat_strings_with_empty_strings)
{
    char* result = cat_strings(4, "", "", "", "");
    EXPECT_THAT(strcmp(result, ""), Eq(0));
    free_string(result);

    result = cat_strings(6, "", "", "hel", "", "lo", "");
    EXPECT_THAT(strcmp(result, "hello"), Eq(0));
    free_string(result);
}


TEST(NAME, cat_strings_with_null_ptr)
{
    char* result = cat_strings(4, "", "", NULL, "");
    EXPECT_THAT(strcmp(result, ""), Eq(0));
    free_string(result);

    result = cat_strings(4, NULL, NULL, NULL, NULL);
    EXPECT_THAT(strcmp(result, ""), Eq(0));
    free_string(result);
}

TEST(NAME, cat_strings_with_no_strings)
{
    char* result = cat_strings(0);
    EXPECT_THAT(strcmp(result, ""), Eq(0));
    free_string(result);
}

TEST(NAME, malloc_string)
{
    char* result = malloc_string("this is a test");
    EXPECT_THAT(strcmp(result, "this is a test"), Eq(0));
    free_string(result);
}

TEST(NAME, malloc_string_empty_string)
{
    char* result = malloc_string("");
    EXPECT_THAT(strcmp(result, ""), Eq(0));
    free_string(result);
}

TEST(NAME, cat_wstrings)
{
    wchar_t* result = cat_wstrings(4, L"this", L" is", L" a", L" test");
    EXPECT_THAT(wcscmp(result, L"this is a test"), Eq(0));
    free_string(result);
}

TEST(NAME, cat_wstrings_with_empty_strings)
{
    wchar_t* result = cat_wstrings(4, L"", L"", L"", L"");
    EXPECT_THAT(wcscmp(result, L""), Eq(0));
    free_string(result);

    result = cat_wstrings(6, L"", L"", L"hel", L"", L"lo", L"");
    EXPECT_THAT(wcscmp(result, L"hello"), Eq(0));
    free_string(result);
}

TEST(NAME, cat_wstrings_with_null_ptr)
{
    wchar_t* result = cat_wstrings(4, L"", L"", NULL, L"");
    EXPECT_THAT(wcscmp(result, L""), Eq(0));
    free_string(result);

    result = cat_wstrings(4, NULL, NULL, NULL, NULL);
    EXPECT_THAT(wcscmp(result, L""), Eq(0));
    free_string(result);
}

TEST(NAME, cat_wstrings_with_no_strings)
{
    wchar_t* result = cat_wstrings(0);
    EXPECT_THAT(wcscmp(result, L""), Eq(0));
    free_string(result);
}

TEST(NAME, malloc_wstring)
{
    wchar_t* result = malloc_wstring(L"this is a test");
    EXPECT_THAT(wcscmp(result, L"this is a test"), Eq(0));
    free_string(result);
}

TEST(NAME, malloc_wstring_empty_string)
{
    wchar_t* result = malloc_wstring(L"");
    EXPECT_THAT(wcscmp(result, L""), Eq(0));
    free_string(result);
}

TEST(NAME, string_to_wide_string)
{
    wchar_t* result = strtowcs("this is a test");
    EXPECT_THAT(wcscmp(result, L"this is a test"), Eq(0));
    free_string(result);
}

TEST(NAME, string_to_wide_string_empty_string)
{
    wchar_t* result = strtowcs("");
    EXPECT_THAT(wcscmp(result, L""), Eq(0));
    free_string(result);
}

TEST(NAME, wide_string_to_string)
{
    char* result = wcstostr(L"this is a test");
    EXPECT_THAT(strcmp(result, "this is a test"), Eq(0));
    free_string(result);
}

TEST(NAME, wide_string_to_string_empty_string)
{
    char* result = wcstostr(L"");
    EXPECT_THAT(strcmp(result, ""), Eq(0));
    free_string(result);
}

TEST(NAME, crlf2lf)
{
    char* s = malloc_string("this\r\nis\r\na\r\ntest\r\n");

    crlf2lf(s);
    EXPECT_THAT(strcmp(s, "this\nis\na\ntest\n"), Eq(0));

    crlf2lf(s);
    EXPECT_THAT(strcmp(s, "this\nis\na\ntest\n"), Eq(0));

    free_string(s);
}

TEST(NAME, crlf2lf_empty_string)
{
    char* s = malloc_string("");

    crlf2lf(s);
    EXPECT_THAT(strcmp(s, ""), Eq(0));

    free_string(s);
}
