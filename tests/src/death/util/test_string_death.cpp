#include "gmock/gmock.h"
#include "util/string.h"
#include "util/memory.h"

#define NAME string_DeathTest

using namespace testing;

#ifdef _DEBUG
#   ifdef LIGHTSHIP_UTIL_PLATFORM_MACOSX
#       define ASSERTION_REGEX ".*Assertion failed.*"
#   elif defined(LIGHTSHIP_UTIL_PLATFORM_LINUX)
#       define ASSERTION_REGEX "\\bAssertion `\\w+' failed."
#   else
#       define ASSERTION_REGEX ""
#   endif
#else
#   define ASSERTION_REGEX ""
#endif

TEST(NAME, malloc_string_with_null_ptr)
{
    testing::FLAGS_gtest_death_test_style = "threadsafe";
    ASSERT_DEATH(malloc_string(NULL), ASSERTION_REGEX);
}

TEST(NAME, malloc_wstring_with_null_ptr)
{
    testing::FLAGS_gtest_death_test_style = "threadsafe";
    ASSERT_DEATH(malloc_wstring(NULL), ASSERTION_REGEX);
}

TEST(NAME, string_to_wide_string_with_null_ptr)
{
    testing::FLAGS_gtest_death_test_style = "threadsafe";
    ASSERT_DEATH(strtowcs(NULL), ASSERTION_REGEX);
}

TEST(NAME, wide_string_to_string_with_null_ptr)
{
    testing::FLAGS_gtest_death_test_style = "threadsafe";
    ASSERT_DEATH(wcstostr(NULL), ASSERTION_REGEX);
}

TEST(NAME, crlf2lf_with_null_ptr)
{
    testing::FLAGS_gtest_death_test_style = "threadsafe";
    ASSERT_DEATH(crlf2lf(NULL), ASSERTION_REGEX);
}
