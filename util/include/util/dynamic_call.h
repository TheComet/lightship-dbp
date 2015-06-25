/*!
 * @file dynamic_call.h
 * @brief Manages things to do with dynamic type information.
 *
 * In order to be able to call functions with arbitrary amount of arguments
 * and arguments who's types aren't known at compile time, a common calling
 * convention must first be established and a method for passing around
 * these arguments as well as being able to assert their correct types during
 * runtime must be implemented.
 *
 * The benefit of this approach is that scripting layers can easily be
 * implemented without the need to wrap all existing functions in order to
 * expose them to scripts.
 *
 * The solution used here is to insert the arguments to be passed into a
 * void** type and pass along a type_info_t object with it. The receiving end
 * can use the type_info_t object to recover the types from the void** type.
 *
 * This can also work the other way around. A void** type can be constructed
 * from varargs or even strings before the function call by using the
 * type_info_t object.
 */

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
 * ```EXTRACT_ARGUMENT_PTR()``^ instead.
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
 * ```EXTRACT_ARGUMENT()```.
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
	TYPE_VOID,

	TYPE_INT8,   /* NOTE: Interleaving signed and unsigned-ness so */
	TYPE_UINT8,  /*       a signed type can be set to an unsigned */
	TYPE_INT16,  /*       type by incrementing by 1. */
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
	type_e* argv_type;          /* argument types */
	type_e ret_type;            /* return type */
	uint32_t argc;              /* number of arguments */
	char has_unknown_types;     /* stores whether or not any of the arguments
								 * or return type are unknown after being
								 * parsed */
};

/*!
 * @brief Creates a type info object based on stringified types.
 *
 * If any of the types are unknown - that is it's not one of the built in C
 * types - then the field ```has_unknown_types``` will be non-zero and the
 * appropriate type will be set to ```TYPE_UNKNOWN```.
 *
 * The following example will create a type info object specifying
 * ```TYPE_VOID``` as the return type and ```TYPE_INT32``` and ```TYPE_FLOAT```
 * as the two argument types:
 * ```
 * struct type_info_t* type_info =
 *         dynamic_call_create_type_info("void", 2, "int", "float");
 * ```
 * @param ret_type A stringified version of the return type.
 * @param argc The number of argument types.
 * @param argv An array of stringified argument types.
 * @note The number of strings in argv must equal ```argc```.
 */
LIGHTSHIP_UTIL_PUBLIC_API struct type_info_t*
dynamic_call_create_type_info(const char* ret_type,
							  int argc,
							  const char** argv);

/*!
 * @brief Destroys a previously created type info object.
 * @param type_info The type info object to destroy.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
dynamic_call_destroy_type_info(struct type_info_t* type_info);

/*!
 * @brief Allocates and initialises a new argument vector from varargs, which
 * can be used in a dynamic call.
 * @param type_info The type information to use in order to deduce the types of
 * the variadic arguments.
 *
 * The types of the arguments passed to this variadic function are deduced
 * according to what the type_info object specifies. The arguments are inserted
 * into the returned argument vector accordingly.
 *
 * Example:
 * ```
 * int a = 8;
 * void** argv = dynamic_call_create_argument_vector_from_varargs(type_info, a);
 * ```
 * @warning Please understand that the type of each variadic argument passed
 * **must be precisely equal to the expected type specified by the type info
 * object**. It is **dangerous** to use immediate values directly as arguments
 * because of type promotion.
 */
LIGHTSHIP_UTIL_PUBLIC_API void**
dynamic_call_create_argument_vector_from_varargs(
		const struct type_info_t* type_info, ...);

/*!
 * @brief @see dynamic_call_create_argument_vector_from_varargs().
 */
LIGHTSHIP_UTIL_PUBLIC_API void**
vdynamic_call_create_argument_vector_from_varargs(
		const struct type_info_t* type_info, va_list ap);

/*!
 * @brief Writes new values into an existing argument vector from varargs.
 * @param type_info The type information to use in order to deduce the types of
 * the variadic arguments.
 *
 * The types of the arguments passed to this variadic function are deduced
 * according to what the type_info object specifies. The arguments are inserted
 * into the returned argument vector accordingly.
 *
 * Example:
 * ```
 * int a = 8;
 * void** argv = dynamic_call_create_argument_vector_from_varargs(type_info, a);
 * ```
 * @warning Please understand that the type of each variadic argument passed
 * **must be precisely equal to the expected type specified by the type info
 * object**. It is **dangerous** to use immediate values directly as arguments
 * because of type promotion.
 * @param argv The existing argument vector to write to.
 */
LIGHTSHIP_UTIL_PUBLIC_API char
dynamic_call_set_argument_vector_from_varargs(
		const struct type_info_t* type_info, void** argv,  ...);

/*!
 * @brief @see dynamic_call_set_argument_vector_from_varargs().
 */
LIGHTSHIP_UTIL_PUBLIC_API char
vdynamic_call_set_argument_vector_from_varargs(
		const struct type_info_t* type_info, void** argv, va_list ap);

/*!
 * @brief Allocates and initialises a new argument vector from string arguments
 * , which can be used in a dynamic call.
 * @param type_info The type information to use in order to know how to parse
 * each string.
 *
 * Example:
 * ```
 * ordered_vector_t* vec = ordered_vector_create(sizeof(char*));
 * ordered_vector_push_emplace(vec) = "5.678";
 * void** argv = dynamic_call_create_argument_vector_from_varargs(type_info, vec);
 * ```
 * Assuming type_info specifies the first argument to be of type ```TYPE_FLOAT```
 * the string ```"5.678"``` will be converted to a float and inserted into
 * the returned argument vector.
 * @param argv A vector (of type ```ordered_vector_t```) filled with a list of
 * strings.
 * @note We opted to use a vector instead of a raw C array of strings so the
 * size is known when building the argument vector.
 */
LIGHTSHIP_UTIL_PUBLIC_API void**
dynamic_call_create_argument_vector_from_strings(
		const struct type_info_t* type_info,
		const struct ordered_vector_t* argv);

/*!
 * @brief Writes new values to an existing argument vector from a list of strings.
 * @param type_info The type information to use in order to know how to parse
 * each string.
 *
 * Example:
 * ```
 * ordered_vector_t* vec = ordered_vector_create(sizeof(char*));
 * ordered_vector_push_emplace(vec) = "5.678";
 * void** argv = dynamic_call_create_argument_vector_from_varargs(type_info, vec);
 * ```
 * Assuming type_info specifies the first argument to be of type ```TYPE_FLOAT```
 * the string ```"5.678"``` will be converted to a float and inserted into
 * the returned argument vector.
 * @param argv A vector (of type ```ordered_vector_t```) filled with a list of
 * strings.
 * @note We opted to use a vector instead of a raw C array of strings so the
 * size is known when building the argument vector.
 */
LIGHTSHIP_UTIL_PUBLIC_API char
dynamic_call_set_argument_vector_from_strings(
	const struct type_info_t* type_info,
	void** argv,
	const struct ordered_vector_t* strings);

/*!
 * @brief Destroys a previously created argument vector.
 * @param type_info The type info object that was used when creating the
 * argument vector.
 * @param argv The argument vector to destroy.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
dynamic_call_destroy_argument_vector(const struct type_info_t* type_info,
									 void** argv);

/*!
 * @brief
 */
LIGHTSHIP_UTIL_PUBLIC_API char
dynamic_call_do_typecheck(const struct type_info_t* type_info,
						  const char* ret_type,
						  uint32_t argc,
						  const char** argv);

/*!
 * @brief Parses a stringified type and returns its type according to the enum
 * ```type_e```.
 */
LIGHTSHIP_UTIL_PUBLIC_API type_e
dynamic_call_get_type_from_string(const char* type);

C_HEADER_END

#endif /* LIGHTSHIP_UTIL_DYNAMIC_CALL_H */
