#include "gmock/gmock.h"
#include "framework/game.h"
#include "framework/services.h"

#define NAME services

using namespace testing;

TEST(NAME, get_type_from_string_test_strings)
{
    EXPECT_THAT(service_get_type_from_string("char"),           Eq(SERVICE_TYPE_INT8));
    EXPECT_THAT(service_get_type_from_string("unsigned char"),  Eq(SERVICE_TYPE_UINT8));
    EXPECT_THAT(service_get_type_from_string("char*"),          Eq(SERVICE_TYPE_STRING));
    EXPECT_THAT(service_get_type_from_string("char**"),         Eq(SERVICE_TYPE_UNKNOWN));
    EXPECT_THAT(service_get_type_from_string("wchar_t"),        Eq(SERVICE_TYPE_UNKNOWN));
    EXPECT_THAT(service_get_type_from_string("wchar_t*"),       Eq(SERVICE_TYPE_WSTRING));
    EXPECT_THAT(service_get_type_from_string("wchar_t**"),      Eq(SERVICE_TYPE_UNKNOWN));
}

TEST(NAME, get_type_from_string_test_integers)
{
    EXPECT_THAT(service_get_type_from_string("int"),            Eq(SERVICE_TYPE_INT32));
    EXPECT_THAT(service_get_type_from_string("int8_t"),         Eq(SERVICE_TYPE_INT8));
    EXPECT_THAT(service_get_type_from_string("int16_t"),        Eq(SERVICE_TYPE_INT16));
    EXPECT_THAT(service_get_type_from_string("int32_t"),        Eq(SERVICE_TYPE_INT32));
    EXPECT_THAT(service_get_type_from_string("int64_t"),        Eq(SERVICE_TYPE_INT64));
    EXPECT_THAT(service_get_type_from_string("unsigned int"),   Eq(SERVICE_TYPE_UINT32));
    EXPECT_THAT(service_get_type_from_string("uint8_t"),        Eq(SERVICE_TYPE_UINT8));
    EXPECT_THAT(service_get_type_from_string("uint16_t"),       Eq(SERVICE_TYPE_UINT16));
    EXPECT_THAT(service_get_type_from_string("uint32_t"),       Eq(SERVICE_TYPE_UINT32));
    EXPECT_THAT(service_get_type_from_string("uint64_t"),       Eq(SERVICE_TYPE_UINT64));
    EXPECT_THAT(service_get_type_from_string("unsigned int8_t"), Eq(SERVICE_TYPE_UINT8));
    EXPECT_THAT(service_get_type_from_string("unsigned int16_t"),Eq(SERVICE_TYPE_UINT16));
    EXPECT_THAT(service_get_type_from_string("unsigned int32_t"),Eq(SERVICE_TYPE_UINT32));
    EXPECT_THAT(service_get_type_from_string("unsigned int64_t"),Eq(SERVICE_TYPE_UINT64));

    EXPECT_THAT(service_get_type_from_string("int*"),           Eq(SERVICE_TYPE_UNKNOWN));
    EXPECT_THAT(service_get_type_from_string("int8_t*"),        Eq(SERVICE_TYPE_UNKNOWN));
    EXPECT_THAT(service_get_type_from_string("int16_t*"),       Eq(SERVICE_TYPE_UNKNOWN));
    EXPECT_THAT(service_get_type_from_string("int32_t*"),       Eq(SERVICE_TYPE_UNKNOWN));
    EXPECT_THAT(service_get_type_from_string("int64_t*"),       Eq(SERVICE_TYPE_UNKNOWN));
    EXPECT_THAT(service_get_type_from_string("unsigned int*"),  Eq(SERVICE_TYPE_UNKNOWN));
    EXPECT_THAT(service_get_type_from_string("uint8_t*"),       Eq(SERVICE_TYPE_UNKNOWN));
    EXPECT_THAT(service_get_type_from_string("uint16_t*"),      Eq(SERVICE_TYPE_UNKNOWN));
    EXPECT_THAT(service_get_type_from_string("uint32_t*"),      Eq(SERVICE_TYPE_UNKNOWN));
    EXPECT_THAT(service_get_type_from_string("uint64_t*"),      Eq(SERVICE_TYPE_UNKNOWN));
    EXPECT_THAT(service_get_type_from_string("unsigned int8_t*"), Eq(SERVICE_TYPE_UNKNOWN));
    EXPECT_THAT(service_get_type_from_string("unsigned int16_t*"),Eq(SERVICE_TYPE_UNKNOWN));
    EXPECT_THAT(service_get_type_from_string("unsigned int32_t*"),Eq(SERVICE_TYPE_UNKNOWN));
    EXPECT_THAT(service_get_type_from_string("unsigned int64_t*"),Eq(SERVICE_TYPE_UNKNOWN));
}

TEST(NAME, get_type_from_string_test_floating_point)
{
    EXPECT_THAT(service_get_type_from_string("float"),          Eq(SERVICE_TYPE_FLOAT));
    EXPECT_THAT(service_get_type_from_string("double"),         Eq(SERVICE_TYPE_DOUBLE));
    EXPECT_THAT(service_get_type_from_string("float*"),         Eq(SERVICE_TYPE_UNKNOWN));
    EXPECT_THAT(service_get_type_from_string("double*"),        Eq(SERVICE_TYPE_UNKNOWN));
}

TEST(NAME, get_type_from_string_test_void)
{
    EXPECT_THAT(service_get_type_from_string("void"),           Eq(SERVICE_TYPE_NONE));
    EXPECT_THAT(service_get_type_from_string("void*"),          Eq(SERVICE_TYPE_UNKNOWN));
}

TEST(NAME, get_type_from_string_test_unknowns)
{
    EXPECT_THAT(service_get_type_from_string("short"),          Eq(SERVICE_TYPE_UNKNOWN));
    EXPECT_THAT(service_get_type_from_string("long"),           Eq(SERVICE_TYPE_UNKNOWN));
    EXPECT_THAT(service_get_type_from_string("unsigned short"), Eq(SERVICE_TYPE_UNKNOWN));
    EXPECT_THAT(service_get_type_from_string("unsigned long"),  Eq(SERVICE_TYPE_UNKNOWN));
}
