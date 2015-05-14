#include "gmock/gmock.h"
#include "util/string.h"
#include "util/memory.h"

#define NAME string_DeathTest

using namespace testing;

TEST(NAME, malloc_string_with_null_ptr)
{
    testing::FLAGS_gtest_death_test_style = "threadsafe";
    ASSERT_DEATH(malloc_string(NULL), "\\bAssertion `\\w+' failed.");
}

TEST(NAME, malloc_wstring_with_null_ptr)
{
    testing::FLAGS_gtest_death_test_style = "threadsafe";
    ASSERT_DEATH(malloc_wstring(NULL), "\\bAssertion `\\w+' failed.");
}

TEST(NAME, string_to_wide_string_with_null_ptr)
{
    testing::FLAGS_gtest_death_test_style = "threadsafe";
    ASSERT_DEATH(strtowcs(NULL), "\\bAssertion `\\w+' failed.");
}

TEST(NAME, wide_string_to_string_with_null_ptr)
{
    testing::FLAGS_gtest_death_test_style = "threadsafe";
    ASSERT_DEATH(wcstostr(NULL), "\\bAssertion `\\w+' failed.");
}

TEST(NAME, crlf2lf_with_null_ptr)
{
    testing::FLAGS_gtest_death_test_style = "threadsafe";
    ASSERT_DEATH(crlf2lf(NULL), "\\bAssertion `\\w+' failed.");
}
