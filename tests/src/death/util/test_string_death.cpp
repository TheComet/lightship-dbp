#include "tests/globals.hpp"
#include "gmock/gmock.h"
#include "util/string.h"
#include "util/memory.h"

#define NAME string_DeathTest

using namespace testing;

TEST(NAME, malloc_string_with_null_ptr)
{
    ASSERT_DEATH(malloc_string(NULL), ASSERTION_REGEX);
}

TEST(NAME, malloc_wstring_with_null_ptr)
{
    ASSERT_DEATH(malloc_wstring(NULL), ASSERTION_REGEX);
}

TEST(NAME, string_to_wide_string_with_null_ptr)
{
    ASSERT_DEATH(strtowcs(NULL), ASSERTION_REGEX);
}

TEST(NAME, wide_string_to_string_with_null_ptr)
{
    ASSERT_DEATH(wcstostr(NULL), ASSERTION_REGEX);
}

TEST(NAME, crlf2lf_with_null_ptr)
{
    ASSERT_DEATH(crlf2lf(NULL), ASSERTION_REGEX);
}

TEST(NAME, reverse_with_null_ptr)
{
    ASSERT_DEATH(string_reverse(NULL), ASSERTION_REGEX);
}
