#include "gmock/gmock.h"
#include "util/dynamic_call.h"

#define NAME dynamic_call

using namespace testing;

TEST(NAME, get_type_from_string_test_strings)
{
    EXPECT_THAT(dynamic_call_get_type_from_string("char"),           Eq(TYPE_INT8));
    EXPECT_THAT(dynamic_call_get_type_from_string("unsigned char"),  Eq(TYPE_UINT8));
    EXPECT_THAT(dynamic_call_get_type_from_string("char*"),          Eq(TYPE_STRING));
    EXPECT_THAT(dynamic_call_get_type_from_string("char**"),         Eq(TYPE_UNKNOWN));
    EXPECT_THAT(dynamic_call_get_type_from_string("wchar_t"),        Eq(TYPE_UNKNOWN));
    EXPECT_THAT(dynamic_call_get_type_from_string("wchar_t*"),       Eq(TYPE_WSTRING));
    EXPECT_THAT(dynamic_call_get_type_from_string("wchar_t**"),      Eq(TYPE_UNKNOWN));
}

TEST(NAME, get_type_from_string_test_integers)
{
    EXPECT_THAT(dynamic_call_get_type_from_string("int"),            Eq(TYPE_INT32));
    EXPECT_THAT(dynamic_call_get_type_from_string("int8_t"),         Eq(TYPE_INT8));
    EXPECT_THAT(dynamic_call_get_type_from_string("int16_t"),        Eq(TYPE_INT16));
    EXPECT_THAT(dynamic_call_get_type_from_string("int32_t"),        Eq(TYPE_INT32));
    EXPECT_THAT(dynamic_call_get_type_from_string("int64_t"),        Eq(TYPE_INT64));
    EXPECT_THAT(dynamic_call_get_type_from_string("unsigned int"),   Eq(TYPE_UINT32));
    EXPECT_THAT(dynamic_call_get_type_from_string("uint8_t"),        Eq(TYPE_UINT8));
    EXPECT_THAT(dynamic_call_get_type_from_string("uint16_t"),       Eq(TYPE_UINT16));
    EXPECT_THAT(dynamic_call_get_type_from_string("uint32_t"),       Eq(TYPE_UINT32));
    EXPECT_THAT(dynamic_call_get_type_from_string("uint64_t"),       Eq(TYPE_UINT64));
    EXPECT_THAT(dynamic_call_get_type_from_string("unsigned int8_t"), Eq(TYPE_UINT8));
    EXPECT_THAT(dynamic_call_get_type_from_string("unsigned int16_t"),Eq(TYPE_UINT16));
    EXPECT_THAT(dynamic_call_get_type_from_string("unsigned int32_t"),Eq(TYPE_UINT32));
    EXPECT_THAT(dynamic_call_get_type_from_string("unsigned int64_t"),Eq(TYPE_UINT64));

    EXPECT_THAT(dynamic_call_get_type_from_string("int*"),           Eq(TYPE_UNKNOWN));
    EXPECT_THAT(dynamic_call_get_type_from_string("int8_t*"),        Eq(TYPE_UNKNOWN));
    EXPECT_THAT(dynamic_call_get_type_from_string("int16_t*"),       Eq(TYPE_UNKNOWN));
    EXPECT_THAT(dynamic_call_get_type_from_string("int32_t*"),       Eq(TYPE_UNKNOWN));
    EXPECT_THAT(dynamic_call_get_type_from_string("int64_t*"),       Eq(TYPE_UNKNOWN));
    EXPECT_THAT(dynamic_call_get_type_from_string("unsigned int*"),  Eq(TYPE_UNKNOWN));
    EXPECT_THAT(dynamic_call_get_type_from_string("uint8_t*"),       Eq(TYPE_UNKNOWN));
    EXPECT_THAT(dynamic_call_get_type_from_string("uint16_t*"),      Eq(TYPE_UNKNOWN));
    EXPECT_THAT(dynamic_call_get_type_from_string("uint32_t*"),      Eq(TYPE_UNKNOWN));
    EXPECT_THAT(dynamic_call_get_type_from_string("uint64_t*"),      Eq(TYPE_UNKNOWN));
    EXPECT_THAT(dynamic_call_get_type_from_string("unsigned int8_t*"), Eq(TYPE_UNKNOWN));
    EXPECT_THAT(dynamic_call_get_type_from_string("unsigned int16_t*"),Eq(TYPE_UNKNOWN));
    EXPECT_THAT(dynamic_call_get_type_from_string("unsigned int32_t*"),Eq(TYPE_UNKNOWN));
    EXPECT_THAT(dynamic_call_get_type_from_string("unsigned int64_t*"),Eq(TYPE_UNKNOWN));
}

TEST(NAME, get_type_from_string_test_floating_point)
{
    EXPECT_THAT(dynamic_call_get_type_from_string("float"),          Eq(TYPE_FLOAT));
    EXPECT_THAT(dynamic_call_get_type_from_string("double"),         Eq(TYPE_DOUBLE));
    EXPECT_THAT(dynamic_call_get_type_from_string("float*"),         Eq(TYPE_UNKNOWN));
    EXPECT_THAT(dynamic_call_get_type_from_string("double*"),        Eq(TYPE_UNKNOWN));
}

TEST(NAME, get_type_from_string_test_void)
{
    EXPECT_THAT(dynamic_call_get_type_from_string("void"),           Eq(TYPE_NONE));
    EXPECT_THAT(dynamic_call_get_type_from_string("void*"),          Eq(TYPE_UNKNOWN));
}

TEST(NAME, get_type_from_string_test_unknowns)
{
    EXPECT_THAT(dynamic_call_get_type_from_string("short"),          Eq(TYPE_UNKNOWN));
    EXPECT_THAT(dynamic_call_get_type_from_string("long"),           Eq(TYPE_UNKNOWN));
    EXPECT_THAT(dynamic_call_get_type_from_string("unsigned short"), Eq(TYPE_UNKNOWN));
    EXPECT_THAT(dynamic_call_get_type_from_string("unsigned long"),  Eq(TYPE_UNKNOWN));
}
