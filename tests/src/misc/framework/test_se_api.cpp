#include "gmock/gmock.h"
#include "framework/se_api.h"

#define NAME se_api

using namespace testing;

TEST(NAME, directory_name_validation)
{
	EXPECT_THAT(directory_name_is_valid("this.is.valid"),   Ne(0));
	EXPECT_THAT(directory_name_is_valid("this_is.valid"),   Ne(0));
	EXPECT_THAT(directory_name_is_valid("this is invalid"), Eq(0));
	EXPECT_THAT(directory_name_is_valid("this-is.invalid"), Eq(0));
}
