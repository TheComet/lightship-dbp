#include "util/pstdint.h"

/*!
 * @brief Helper macro for extracting an argument from an argument vector in a
 * service or event callback function.
 * @param index The index of the argument, beginning at 0.
 * @param var The identifier to give the extracted variable.
 * @param cast_from The type of the value stored at the specified index.
 * @param cast_to The type to cast the extracted argument to. This is also the
 * type of *var*.
 * @note If the argument being extracted is a pointer type, use
 * EXTRACT_ARG_PTR() instead.
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
#define EXTRACT_ARG(index, var, cast_from, cast_to) \
cast_to var = (cast_to) *(cast_from*)argv[index]

/*!
 * @brief Helper macro for extracting a pointer argument from an argument
 * vector in a service function.
 * @param index The index of the argument, beginning at 0.
 * @param var The identifier to give the extracted variable.
 * @param cast_to The pointer type to cast the extracted argument to. This is
 * also the type of *var*.
 * @note If the argument being extracted is not a pointer, use
 * EXTRACT_ARG().
 */
#define EXTRACT_ARG_PTR(index, var, cast_to) \
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
