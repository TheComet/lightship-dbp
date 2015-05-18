#include "gmock/gmock.h"
#include "util/dynamic_call.h"
#include "util/ordered_vector.h"

#define NAME dynamic_call

using namespace testing;

DYNAMIC_FUNCTION(callback)
{
    EXTRACT_ARGUMENT_PTR(0, a, const char*);
    EXTRACT_ARGUMENT_PTR(1, b, const wchar_t*);
    EXTRACT_ARGUMENT(2, c, int8_t,      int64_t);
    EXTRACT_ARGUMENT(3, d, uint8_t,     int64_t);
    EXTRACT_ARGUMENT(4, e, int16_t,     int64_t);
    EXTRACT_ARGUMENT(5, f, uint16_t,    int64_t);
    EXTRACT_ARGUMENT(6, g, int32_t,     int64_t);
    EXTRACT_ARGUMENT(7, h, uint32_t,    int64_t);
    EXTRACT_ARGUMENT(8, i, int64_t,     int64_t);
    EXTRACT_ARGUMENT(9, j, uint64_t,    int64_t);
    EXTRACT_ARGUMENT(10, k, intptr_t,    int64_t);
    EXTRACT_ARGUMENT(11, l, uintptr_t,   int64_t);
    EXTRACT_ARGUMENT(12, m, float,   float);
    EXTRACT_ARGUMENT(13, n, double,  double);

    RETURN(strlen(a) + wcslen(b) + c + d + e + f + g + h + i + j + k + l + m + n, int);
}

TEST(NAME, create_type_info_for_callback)
{
    const char* argvstr[] = {
        "char*", "wchar_t*", "int8_t", "uint8_t", "int16_t", "uint16_t",
        "int32_t", "uint32_t", "int64_t", "uint64_t", "intptr_t", "uintptr_t",
        "float", "double"
    };
    struct type_info_t* t = dynamic_call_create_type_info("int", 14, argvstr);
    ASSERT_THAT(t, NotNull());
    EXPECT_THAT(t->argc, Eq(14));

    EXPECT_THAT(t->argv_type[0], Eq(TYPE_STRING));
    EXPECT_THAT(t->argv_type[1], Eq(TYPE_WSTRING));
    EXPECT_THAT(t->argv_type[2], Eq(TYPE_INT8));
    EXPECT_THAT(t->argv_type[3], Eq(TYPE_UINT8));
    EXPECT_THAT(t->argv_type[4], Eq(TYPE_INT16));
    EXPECT_THAT(t->argv_type[5], Eq(TYPE_UINT16));
    EXPECT_THAT(t->argv_type[6], Eq(TYPE_INT32));
    EXPECT_THAT(t->argv_type[7], Eq(TYPE_UINT32));
    EXPECT_THAT(t->argv_type[8], Eq(TYPE_INT64));
    EXPECT_THAT(t->argv_type[9], Eq(TYPE_UINT64));
    EXPECT_THAT(t->argv_type[10], Eq(TYPE_INTPTR));
    EXPECT_THAT(t->argv_type[11], Eq(TYPE_UINTPTR));
    EXPECT_THAT(t->argv_type[12], Eq(TYPE_FLOAT));
    EXPECT_THAT(t->argv_type[13], Eq(TYPE_DOUBLE));
    EXPECT_THAT(t->ret_type, Eq(TYPE_INT32));

    EXPECT_THAT(t->argv_type_str[0], StrEq("char*"));
    EXPECT_THAT(t->argv_type_str[1], StrEq("wchar_t*"));
    EXPECT_THAT(t->argv_type_str[2], StrEq("int8_t"));
    EXPECT_THAT(t->argv_type_str[3], StrEq("uint8_t"));
    EXPECT_THAT(t->argv_type_str[4], StrEq("int16_t"));
    EXPECT_THAT(t->argv_type_str[5], StrEq("uint16_t"));
    EXPECT_THAT(t->argv_type_str[6], StrEq("int32_t"));
    EXPECT_THAT(t->argv_type_str[7], StrEq("uint32_t"));
    EXPECT_THAT(t->argv_type_str[8], StrEq("int64_t"));
    EXPECT_THAT(t->argv_type_str[9], StrEq("uint64_t"));
    EXPECT_THAT(t->argv_type_str[10], StrEq("intptr_t"));
    EXPECT_THAT(t->argv_type_str[11], StrEq("uintptr_t"));
    EXPECT_THAT(t->argv_type_str[12], StrEq("float"));
    EXPECT_THAT(t->argv_type_str[13], StrEq("double"));
    EXPECT_THAT(t->ret_type_str, StrEq("int"));

    EXPECT_THAT(t->has_unknown_types, Eq(0));

    dynamic_call_destroy_type_info(t);
}

TEST(NAME, create_type_info_void)
{
    struct type_info_t* t = dynamic_call_create_type_info("void", 0, NULL);
    ASSERT_THAT(t, NotNull());
    EXPECT_THAT(t->argc, Eq(0));
    EXPECT_THAT(t->ret_type, Eq(TYPE_NONE));
    EXPECT_THAT(t->ret_type_str, StrEq("void"));
    EXPECT_THAT(t->has_unknown_types, Eq(0));
    dynamic_call_destroy_type_info(t);
}

TEST(NAME, create_type_info_unknown_ret_type)
{
    const char* argv[] = {"void"};
    struct type_info_t* t = dynamic_call_create_type_info("struct x*", 1, argv);
    ASSERT_THAT(t, NotNull());
    EXPECT_THAT(t->argc, Eq(1));
    EXPECT_THAT(t->ret_type, Eq(TYPE_UNKNOWN));
    EXPECT_THAT(t->ret_type_str, StrEq("struct x*"));
    EXPECT_THAT(t->argv_type[0], Eq(TYPE_NONE));
    EXPECT_THAT(t->argv_type_str[0], StrEq("void"));
    EXPECT_THAT(t->has_unknown_types, Eq(1));
    dynamic_call_destroy_type_info(t);
}

TEST(NAME, create_type_info_unknown_argv_type)
{
    const char* argv[] = {"struct x*"};
    struct type_info_t* t = dynamic_call_create_type_info("void", 1, argv);
    ASSERT_THAT(t, NotNull());
    EXPECT_THAT(t->argc, Eq(1));
    EXPECT_THAT(t->ret_type, Eq(TYPE_NONE));
    EXPECT_THAT(t->ret_type_str, StrEq("void"));
    EXPECT_THAT(t->argv_type[0], Eq(TYPE_UNKNOWN));
    EXPECT_THAT(t->argv_type_str[0], StrEq("struct x*"));
    EXPECT_THAT(t->has_unknown_types, Eq(1));
    dynamic_call_destroy_type_info(t);
}

TEST(NAME, callback_raw)
{
    /* create type_info for function */
    const char* argvstr[] = {
        "char*", "wchar_t*", "int8_t", "uint8_t", "int16_t", "uint16_t",
        "int32_t", "uint32_t", "int64_t", "uint64_t", "intptr_t", "uintptr_t",
        "float", "double"
    };
    struct type_info_t* t = dynamic_call_create_type_info("int", 14, argvstr);
    ASSERT_THAT(t, NotNull());

    /* create return type and argument vector */
    int ret;
    const char* a = "testing!";
    const wchar_t* b = L"hello";
    int8_t c = 3;
    uint8_t d = 2;
    int16_t e = 8;
    uint16_t f = 2;
    int32_t g = 5;
    uint32_t h = 6;
    int64_t i = 3;
    uint64_t j = 7;
    intptr_t k = 4;
    uintptr_t l = 7;
    float m = 10;
    double n = 45;

    void* argv[14];
    argv[0] = &(PTR(a));
    argv[1] = &(PTR(b));
    argv[2] = &c;
    argv[3] = &d;
    argv[4] = &e;
    argv[5] = &f;
    argv[6] = &g;
    argv[7] = &h;
    argv[8] = &i;
    argv[9] = &j;
    argv[10] = &k;
    argv[11] = &l;
    argv[12] = &m;
    argv[13] = &n;

    /* call */
    callback(&ret, argv);
    EXPECT_THAT(ret, Eq(8 + 5 + 3 + 2 + 8 + 2 + 5 + 6 + 3 + 7 + 4 + 7 + 10 + 45));

    dynamic_call_destroy_type_info(t);
}

TEST(NAME, callback_create_argument_vector_from_varargs)
{
    /* create type_info for function */
    const char* argvstr[] = {
        "char*", "wchar_t*", "int8_t", "uint8_t", "int16_t", "uint16_t",
        "int32_t", "uint32_t", "int64_t", "uint64_t", "intptr_t", "uintptr_t",
        "float", "double"
    };
    struct type_info_t* t = dynamic_call_create_type_info("int", 14, argvstr);
    ASSERT_THAT(t, NotNull());

    /* create return type and argument vector */
    int ret;
    void** argv = dynamic_call_create_argument_vector_from_varargs(t,
        "another test!",
        L"lolololol",
        3, 6, 3, 8, 9, 4, 2, 4, 3, 6, 4.0, 65.0
    );

    /* call */
    callback(&ret, argv);
    EXPECT_THAT(ret, Eq(13 + 9 + 3 + 6 + 3 + 8 + 9 + 4 + 2 + 4 + 3 + 6 + 4.0 + 65.0));

    dynamic_call_destroy_argument_vector(t, argv);
    dynamic_call_destroy_type_info(t);
}

TEST(NAME, callback_create_argument_vector_from_strings)
{
    /* create type_info for function */
    const char* argvstr[] = {
        "char*", "wchar_t*", "int8_t", "uint8_t", "int16_t", "uint16_t",
        "int32_t", "uint32_t", "int64_t", "uint64_t", "intptr_t", "uintptr_t",
        "float", "double"
    };
    struct type_info_t* t = dynamic_call_create_type_info("int", 14, argvstr);
    ASSERT_THAT(t, NotNull());

    /* create return type and argument vector */
    int ret;
    struct ordered_vector_t argv_params;
    const char* args[] = {"test", "hello", "3", "6", "3", "8", "9", "4", "2", "4", "3", "6", "4.0", "65.0"};
    ordered_vector_init_vector(&argv_params, sizeof(char*));
    for(int i = 0; i != 14; ++i)
        ordered_vector_push(&argv_params, &args[i]);

    void** argv = dynamic_call_create_argument_vector_from_strings(t, &argv_params);
    ASSERT_THAT(argv, NotNull());

    /* call */
    callback(&ret, argv);
    EXPECT_THAT(ret, Eq(4 + 5 + 3 + 6 + 3 + 8 + 9 + 4 + 2 + 4 + 3 + 6 + 4.0 + 65.0));

    ordered_vector_clear_free(&argv_params);
    dynamic_call_destroy_argument_vector(t, argv);
    dynamic_call_destroy_type_info(t);
}

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
