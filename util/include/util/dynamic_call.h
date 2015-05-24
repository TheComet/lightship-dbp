#ifndef LIGHTSHIP_UTIL_DYNAMIC_CALL_H
#define LIGHTSHIP_UTIL_DYNAMIC_CALL_H

#include <stdarg.h>
#include "util/pstdint.h"
#include "util/config.h"

C_HEADER_BEGIN

struct ordered_vector_t;

/*!
 * @brief Helper macro for extracting an argument from an argument vector in a
 * service or event callback function.
 * @param index The index of the argument, beginning at 0.
 * @param var The identifier to give the extracted variable.
 * @param cast_from The type of the value stored at the specified index.
 * @param cast_to The type to cast the extracted argument to. This is also the
 * type of *var*.
 * @note If the argument being extracted is a pointer type, use
 * EXTRACT_ARGUMENT_PTR() instead.
 * @note *cast_from* and *cast_to* are necessary because there are two
 * dereferences. You can think of it as the extracted value being stored into
 * an intermediate before being casted to its final type. The type it has in
 * its intermediate state must be equal to the type it had when it was first
 * inserted into the argument vector. Most of the time, *cast_from* and
 * *cast_to* will be identical, but there are (rare) cases where they differ,
 * namely in the renderer plugin with floats: Internally, floats are of type
 * GLfloat, but because the caller does not have access to GLfloat, he will
 * pass it as a normal float. In order for the cast from float to GLfloat to be
 * successful, the void-pointer must first be cast to a float-pointer
 * (*cast_from*), and finally cast to a GLfloat-pointer (*cast_to*).
 */
#define EXTRACT_ARGUMENT(index, var, cast_from, cast_to) \
        cast_to var = (cast_to) *(cast_from*)argv[index]

/*!
 * @brief Helper macro for extracting a pointer argument from an argument
 * vector in a service function.
 * @param index The index of the argument, beginning at 0.
 * @param var The identifier to give the extracted variable.
 * @param cast_to The pointer type to cast the extracted argument to. This is
 * also the type of *var*.
 * @note If the argument being extracted is not a pointer, use
 * EXTRACT_ARGUMENT().
 */
#define EXTRACT_ARGUMENT_PTR(index, var, cast_to) \
        cast_to var = (cast_to)argv[index]

/*!
 * @brief Helper macro for returning values from a service function to the
 * caller.
 * @param value The identifier of the variable to return.
 * @param ret_type The type of the return value. This **must** be identical to
 * the type the caller is expecting to be returned. If this is not the cast,
 * the returned value will become undefined.
 */
#define RETURN(value, ret_type) do { \
        *(ret_type*)ret = (ret_type)value; return; } while(0)

/*!
 * @brief All arguments that are of a pointer type must be passed to a
 * SERVICE_CALLx or EVENT_FIREx function using this macro.
 * @param arg The pointer argument to pass to the service function call.
 */
#define PTR(arg) *(char*)arg

/*!
 * @brief Declare or define a dynamic function.
 */
#define DYNAMIC_FUNCTION(func_name)                                         \
        void func_name(void* ret, void** argv)

#define DYNAMIC_CALL0(func_name, ret_value) do {                            \
    func_name(ret_value, NULL); } while(0)
#define DYNAMIC_CALL1(func_name, ret_value, arg1) do {                      \
    void* argv[1];                                                          \
    argv[0] = &arg1;                                                        \
    func_name(ret_value, argv); } while(0)
#define DYNAMIC_CALL2(func_name, ret_value, arg1, arg2) do {                \
    void* argv[2];                                                          \
    argv[0] = &arg1;                                                        \
    argv[1] = &arg2;                                                        \
    func_name(ret_value, argv); } while(0)
#define DYNAMIC_CALL3(func_name, ret_value, arg1, arg2, arg3) do {          \
    void* argv[3];                                                          \
    argv[0] = &arg1;                                                        \
    argv[1] = &arg2;                                                        \
    argv[2] = &arg3;                                                        \
    func_name(ret_value, argv); } while(0)
#define DYNAMIC_CALL4(func_name, ret_value, arg1, arg2, arg3, arg4) do {    \
    void* argv[4];                                                          \
    argv[0] = &arg1;                                                        \
    argv[1] = &arg2;                                                        \
    argv[2] = &arg3;                                                        \
    argv[3] = &arg4;                                                        \
    func_name(ret_value, argv); } while(0)
#define DYNAMIC_CALL5(func_name, ret_value, arg1, arg2, arg3, arg4, arg5) do {\
    void* argv[5];                                                          \
    argv[0] = &arg1;                                                        \
    argv[1] = &arg2;                                                        \
    argv[2] = &arg3;                                                        \
    argv[3] = &arg4;                                                        \
    argv[4] = &arg5;                                                        \
    func_name(ret_value, argv); } while(0)

typedef enum type_e
{
    TYPE_UNKNOWN,
    TYPE_NONE,

    TYPE_INT8,   /* NOTE: Interleaving signed and unsigned-ness so */
    TYPE_UINT8,  /*       a signed type can be set to an unsigned */
    TYPE_INT16,  /*       type simply by adding 1. */
    TYPE_UINT16,
    TYPE_INT32,
    TYPE_UINT32,
    TYPE_INT64,
    TYPE_UINT64,
    TYPE_INTPTR,
    TYPE_UINTPTR,

    TYPE_FLOAT,
    TYPE_DOUBLE,

    TYPE_STRING,
    TYPE_WSTRING
} type_e;

struct type_info_t
{
    char* ret_type_str;         /* return type as a plain string */
    char** argv_type_str;       /* argument types as plain strings */
    type_e* argv_type;          /* argument types */
    type_e ret_type;            /* return type */
    uint32_t argc;              /* number of arguments */
    char has_unknown_types;     /* stores whether or not any of the arguments
                                 * or return type are unknown after being
                                 * parsed */
};

/*!
 *
 */
LIGHTSHIP_UTIL_PUBLIC_API struct type_info_t*
dynamic_call_create_type_info(const char* ret_type, int argc, const char** argv);

/*!
 *
 */
LIGHTSHIP_UTIL_PUBLIC_API void
dynamic_call_destroy_type_info(struct type_info_t* type_info);

/*!
 *
 */
LIGHTSHIP_UTIL_PUBLIC_API void**
dynamic_call_create_argument_vector_from_varargs(const struct type_info_t* type_info,
                                                  ...);
LIGHTSHIP_UTIL_PUBLIC_API void**
vdynamic_call_create_argument_vector_from_varargs(const struct type_info_t* type_info,
                                                  va_list ap);

/*!
 *
 */
LIGHTSHIP_UTIL_PUBLIC_API void**
dynamic_call_create_argument_vector_from_strings(const struct type_info_t* type_info,
                                                 const struct ordered_vector_t* argv);

/*!
 *
 */
LIGHTSHIP_UTIL_PUBLIC_API void
dynamic_call_destroy_argument_vector(const struct type_info_t* type_info,
                                     void** argv);

/*!
 *
 */
LIGHTSHIP_UTIL_PUBLIC_API char
dynamic_call_do_typecheck(const struct type_info_t* type_info,
                          const char* ret_type,
                          uint32_t argc,
                          const char** argv);

/*!
 *
 */
LIGHTSHIP_UTIL_PUBLIC_API type_e
dynamic_call_get_type_from_string(const char* type);

C_HEADER_END

#endif /* LIGHTSHIP_UTIL_DYNAMIC_CALL_H */
