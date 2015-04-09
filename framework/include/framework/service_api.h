#ifndef LIGHTSHIP_UTIL_SERVICE_H
#define LIGHTSHIP_UTIL_SERVICE_H

#include "util/pstdint.h"
#include "framework/config.h"

struct game_t;
struct service_t;

typedef void (*service_callback_func)(struct service_t* service, void* ret, const void** argv);

/*!
 * @brief Helper macro for defining a service function.
 * 
 * This can be used in the header file to create a function prototype as well
 * as in the source file to define the function body.
 * 
 * The resulting function signature defines 3 arguments:
 *   - service: The service object that called the service function. This can
 *              be used to extract the game object (service->game), retrieve
 *              type information about the arguments, or otherwise help
 *              identify information about the service.
 *   - ret    : A void* pointing to a location where a return value can be
 *              written to. See the helper macro SERVICE_RETURN() for more
 *              information on returning values from service functions.
 *   - argv   : An argument vector of void** pointing to the memory locations
 *              where argument types can be read from. See the helper macros
 *              SERVICE_EXTRACT_ARGUMENT() and SERVICE_EXTRACT_ARGUMENT_PTR()
 *              for more information on extracting arguments from service
 *              functions.
 * @param func_name The name of the service function.
 */
#define SERVICE(func_name) \
        void func_name(struct service_t* service, void* ret, const void** argv)

/*!
 * @brief Helper macro for extracting an argument from an argument vector in a
 * service function.
 * @param index The index of the argument, beginning at 0.
 * @param var The identifier to give the extracted variable.
 * @param cast_from The type of the value stored at the specified index.
 * @param cast_to The type to cast the extracted argument to. This is also the
 * type of *var*.
 * @note If the argument being extracted is a pointer type, use
 * SERVICE_EXTRACT_ARGUMENT_PTR() instead.
 * @note *cast_from* and *cast_to* are necessary because there are two
 * dereferences. You can think of it as the extracted value being stored into
 * a "temporary state" before casted to its final type. The type it has in its
 * temporary state must be equal to the type it had when it was first inserted
 * into the argument vector. Most of the time, *cast_from* and *cast_to* will
 * be identical, but there are (rare) cases where they differ, namely in the
 * renderer plugin with floats: Internally, floats are of type GLfloat, but
 * because the caller does not have access to GLfloat, he will pass it as a
 * normal float. In order for the cast from float to GLfloat to be successful,
 * the void-pointer must first be cast to a float-pointer (*cast_from*), and
 * finally cast to a GLfloat-pointer (*cast_to*)
 */
#define SERVICE_EXTRACT_ARGUMENT(index, var, cast_from, cast_to) \
    cast_to var = (cast_to) *(cast_from*)argv[index]

/*!
 * @brief Helper macro for extracting a pointer argument from an argument
 * vector in a service function.
 * @param index The index of the argument, beginning at 0.
 * @param var The identifier to give the extracted variable.
 * @param cast_to The pointer type to cast the extracted argument to. This is
 * also the type of *var*.
 * @note If the argument being extracted is not a pointer, use
 * SERVICE_EXTRACT_ARGUMENT().
 */
#define SERVICE_EXTRACT_ARGUMENT_PTR(index, var, cast_to) \
    cast_to var = (cast_to)argv[index]

/*!
 * @brief Helper macro for returning values from a service function to the
 * caller.
 * @param value The identifier of the variable to return.
 * @param ret_type The type of the return value. This **must** be identical to
 * the type the caller is expecting to be returned. If this is not the cast,
 * the returned value will become undefined.
 */
#define SERVICE_RETURN(value, ret_type) do { \
        *(ret_type*)ret = (ret_type)value; return; } while(0)

/*!
 * @brief Passed to one of the SERVICE_CALL* functions to indicate that the
 * service does not accept return values.
 */
#define SERVICE_NO_RETURN NULL

/*!
 * @brief All arguments that are of a pointer type must be passed to a
 * SERVICE_CALL* function using this macro.
 * @param arg The pointer argument to pass to the service function call.
 */
#define PTR(arg) *(char*)arg

#define SERVICE_CALL0(service, ret_value) do {                                                  \
            ((struct service_t*)service)->exec(service, ret_value, NULL);                       \
        } while(0)
#define SERVICE_CALL1(service, ret_value, arg1) do {                                            \
            const void* service_internal_argv[1];                                               \
            service_internal_argv[0] = &arg1;                                                   \
            ((struct service_t*)service)->exec(service, ret_value, service_internal_argv);      \
        } while(0)
#define SERVICE_CALL2(service, ret_value, arg1, arg2) do {                                      \
            const void* service_internal_argv[2];                                               \
            service_internal_argv[0] = &arg1;                                                   \
            service_internal_argv[1] = &arg2;                                                   \
            ((struct service_t*)service)->exec(service, ret_value, service_internal_argv);      \
        } while(0)
#define SERVICE_CALL3(service, ret_value, arg1, arg2, arg3) do {                                \
            const void* service_internal_argv[3];                                               \
            service_internal_argv[0] = &arg1;                                                   \
            service_internal_argv[1] = &arg2;                                                   \
            service_internal_argv[2] = &arg3;                                                   \
            ((struct service_t*)service)->exec(service, ret_value, service_internal_argv);      \
        } while(0)
#define SERVICE_CALL4(service, ret_value, arg1, arg2, arg3, arg4) do {                          \
            const void* service_internal_argv[4];                                               \
            service_internal_argv[0] = &arg1;                                                   \
            service_internal_argv[1] = &arg2;                                                   \
            service_internal_argv[2] = &arg3;                                                   \
            service_internal_argv[3] = &arg4;                                                   \
            ((struct service_t*)service)->exec(service, ret_value, service_internal_argv);      \
        } while(0)
#define SERVICE_CALL5(service, ret_value, arg1, arg2, arg3, arg4, arg5) do {                    \
            const void* service_internal_argv[5];                                               \
            service_internal_argv[0] = &arg1;                                                   \
            service_internal_argv[1] = &arg2;                                                   \
            service_internal_argv[2] = &arg3;                                                   \
            service_internal_argv[3] = &arg4;                                                   \
            service_internal_argv[4] = &arg5;                                                   \
            ((struct service_t*)service)->exec(service, ret_value, service_internal_argv);      \
        } while(0)
#define SERVICE_CALL6(service, ret_value, arg1, arg2, arg3, arg4, arg5, arg6) do {              \
            const void* service_internal_argv[6];                                               \
            service_internal_argv[0] = &arg1;                                                   \
            service_internal_argv[1] = &arg2;                                                   \
            service_internal_argv[2] = &arg3;                                                   \
            service_internal_argv[3] = &arg4;                                                   \
            service_internal_argv[4] = &arg5;                                                   \
            service_internal_argv[5] = &arg6;                                                   \
            ((struct service_t*)service)->exec(service, ret_value, service_internal_argv);      \
        } while(0)

#define SERVICE_INTERNAL_GET_AND_CHECK(game, service_name)                                      \
        struct service_t* service_internal_service = service_get(game, service_name);           \
        if(!service_internal_service)                                                           \
            llog(LOG_WARNING, game, NULL, 3, "Service \"", service_name, "\" does not exist");        \
        else

#define SERVICE_CALL_NAME0(game, service_name, ret_value) do {                                  \
            SERVICE_INTERNAL_GET_AND_CHECK(game, service_name)                                  \
            SERVICE_CALL0(service_internal_service, ret_value);                                 \
        } while(0)
#define SERVICE_CALL_NAME1(game, service_name, ret_value, arg1) do {                            \
            SERVICE_INTERNAL_GET_AND_CHECK(game, service_name)                                  \
            SERVICE_CALL1(service_internal_service, ret_value, arg1);                           \
        } while(0)
#define SERVICE_CALL_NAME2(game, service_name, ret_value, arg1, arg2) do {                      \
            SERVICE_INTERNAL_GET_AND_CHECK(game, service_name)                                  \
            SERVICE_CALL2(service_internal_service, ret_value, arg1, arg2);                     \
        } while(0)
#define SERVICE_CALL_NAME3(game, service_name, ret_value, arg1, arg2, arg3) do {                \
            SERVICE_INTERNAL_GET_AND_CHECK(game, service_name)                                  \
            SERVICE_CALL3(service_internal_service, ret_value, arg1, arg2, arg3);               \
        } while(0)
#define SERVICE_CALL_NAME4(game, service_name, ret_value, arg1, arg2, arg3, arg4) do {          \
            SERVICE_INTERNAL_GET_AND_CHECK(game, service_name)                                  \
            SERVICE_CALL4(service_internal_service, ret_value, arg1, arg2, arg3, arg4);         \
        } while(0)
#define SERVICE_CALL_NAME5(game, service_name, ret_value, arg1, arg2, arg3, arg4, arg5) do {    \
            SERVICE_INTERNAL_GET_AND_CHECK(game, service_name)                                  \
            SERVICE_CALL5(service_internal_service, ret_value, arg1, arg2, arg3, arg4, arg5);   \
        } while(0)
#define SERVICE_CALL_NAME6(game, service_name, ret_value, arg1, arg2, arg3, arg4, arg5, arg6) do {  \
            SERVICE_INTERNAL_GET_AND_CHECK(game, service_name)                                      \
            SERVICE_CALL6(service_internal_service, ret_value, arg1, arg2, arg3, arg4, arg5, arg6); \
        } while(0)

/* XXX STRINGIFY(TYPEOF(x)) doesn't do what you think it does. *
#if defined(TYPEOF) && defined(_DEBUG)
#   define SERVICE_TYPECHECK0(service, ret_value) \
            const char* service_internal_ret = STRINGIFY(TYPEOF(ret_value)); \
            if(!service_do_typecheck(service, service_internal_ret, 0, NULL)) \
                break;
#   define SERVICE_TYPECHECK1(service, ret_value, arg1) \
            const char* service_internal_ret = STRINGIFY(TYPEOF(ret_value)); \
            const char* service_internal_argv[1] = {STRINGIFY(TYPEOF(arg1))}; \
            if(!service_do_typecheck(service, service_internal_ret, 1, service_internal_argv) \
                break;
#else
#   define SERVICE_TYPECHECK0(service, ret_value)
#   define SERVICE_TYPECHECK1(service, ret_value, arg1)
#endif*/

#define SERVICE_GEN_TYPECHECK0(callback, ret_type) \
            static ret_type (*service_internal_##callback)(void);
#define SERVICE_GEN_TYPECHECK1(callback, ret_type, arg1) \
            static ret_type (*service_internal_##callback)(arg1);

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#define SERVICE_REGISTER0(game, plugin, service_name, callback, ret_type) do {                          \
            service_register(game, plugin, service_name, callback, STRINGIFY(ret_type), 0, NULL);       \
        } while(0)
#define SERVICE_REGISTER1(game, plugin, service_name, callback, ret_type, arg1) do {                    \
            const char* ret = STRINGIFY(ret_type);                                                      \
            const char* argv[] = {STRINGIFY(arg1)};                                                     \
            service_register(game, plugin, service_name, callback, ret, 1, argv);                       \
        } while(0)
#define SERVICE_REGISTER2(game, plugin, service_name, callback, ret_type, arg1, arg2) do {              \
            const char* ret = STRINGIFY(ret_type);                                                      \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2)};                                    \
            service_register(game, plugin, service_name, callback, ret, 2, argv);                       \
        } while(0)
#define SERVICE_REGISTER3(game, plugin, service_name, callback, ret_type, arg1, arg2, arg3) do {        \
            const char* ret = STRINGIFY(ret_type);                                                      \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2), STRINGIFY(arg3)};                   \
            service_register(game, plugin, service_name, callback, ret, 3, argv);                       \
        } while(0)
#define SERVICE_REGISTER4(game, plugin, service_name, callback, ret_type, arg1, arg2, arg3, arg4) do {  \
            const char* ret = STRINGIFY(ret_type);                                                      \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2), STRINGIFY(arg3), STRINGIFY(arg4)};  \
            service_register(game, plugin, service_name, callback, ret, 4, argv);                       \
        } while(0)
#define SERVICE_REGISTER5(game, plugin, service_name, callback, ret_type, arg1, arg2, arg3, arg4, arg5) do {            \
            const char* ret = STRINGIFY(ret_type);                                                                      \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2), STRINGIFY(arg3), STRINGIFY(arg4), STRINGIFY(arg5)}; \
            service_register(game, plugin, service_name, callback, ret, 5, argv);                                       \
        } while(0)
#define SERVICE_REGISTER6(game, plugin, service_name, callback, ret_type, arg1, arg2, arg3, arg4, arg5, arg6) do {            \
            const char* ret = STRINGIFY(ret_type);                                                                      \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2), STRINGIFY(arg3), STRINGIFY(arg4), STRINGIFY(arg5), STRINGIFY(arg6)}; \
            service_register(game, plugin, service_name, callback, ret, 6, argv);                                       \
        } while(0)
/*
#define SERVICE_REGISTER6(game, plugin, service_name, callback, ret_type, arg1, arg2, arg3, arg4, arg5, arg6) do {                      \
            const char* ret = STRINGIFY(ret_type);                                                                                      \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2), STRINGIFY(arg3), STRINGIFY(arg4), STRINGIFY(arg5), STRINGIFY(arg6)};\
            service_register(game, plugin, service_name, callback, ret, 6, argv);                                                       \
        } while(0)*/

struct service_t
{
    struct game_t* game;
    char* name;
    uint32_t argc;
    char* ret_type;
    char** argv_type;
    service_callback_func exec;
};

#endif /* LIGHTSHIP_UTIL_SERVICE_H */
