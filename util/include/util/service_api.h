#ifndef LIGHTSHIP_UTIL_SERVICE_H
#define LIGHTSHIP_UTIL_SERVICE_H

#include "util/pstdint.h"
#include "util/config.h"

struct service_t;
typedef void (*service_callback_func)(void* ret, const void** argv);

LIGHTSHIP_PUBLIC_API extern char g_service_internal_no_arg_dummy;

#define SERVICE(func_name) \
        void func_name(void* ret, const void** argv)

#define SERVICE_EXTRACT_ARGUMENT(index, var, cast_from, cast_to) \
    cast_to var = (cast_to) *(cast_from*)argv[index]
#define SERVICE_EXTRACT_ARGUMENT_PTR(index, var, cast_to) \
    cast_to var = (cast_to)argv[index]

#define SERVICE_RETURN(value, ret_type) do {\
        *(ret_type*)ret = value; return; } while(0)

#define SERVICE_NO_RETURN NULL
#define SERVICE_NO_ARGUMENT g_service_internal_no_arg_dummy

#define SERVICE_CALL0(service, ret_value) do { \
            ((struct service_t*)service)->exec(ret_value, NULL); \
        } while(0)
#define SERVICE_CALL1(service, ret_value, arg1) do { \
            const void* service_internal_argv[1]; \
            service_internal_argv[0] = &arg1; \
            ((struct service_t*)service)->exec(ret_value, service_internal_argv); \
        } while(0)
#define SERVICE_CALL2(service, ret_value, arg1, arg2) do { \
            const void* service_internal_argv[2]; \
            service_internal_argv[0] = &arg1; \
            service_internal_argv[1] = &arg2; \
            ((struct service_t*)service)->exec(ret_value, service_internal_argv); \
        } while(0)
#define SERVICE_CALL3(service, ret_value, arg1, arg2, arg3) do { \
            const void* service_internal_argv[3]; \
            service_internal_argv[0] = &arg1; \
            service_internal_argv[1] = &arg2; \
            service_internal_argv[2] = &arg3; \
            ((struct service_t*)service)->exec(ret_value, service_internal_argv); \
        } while(0)
#define SERVICE_CALL4(service, ret_value, arg1, arg2, arg3, arg4) do { \
            const void* service_internal_argv[4]; \
            service_internal_argv[0] = &arg1; \
            service_internal_argv[1] = &arg2; \
            service_internal_argv[2] = &arg3; \
            service_internal_argv[3] = &arg4; \
            ((struct service_t*)service)->exec(ret_value, service_internal_argv); \
        } while(0)
#define SERVICE_CALL5(service, ret_value, arg1, arg2, arg3, arg4, arg5) do { \
            const void* service_internal_argv[5]; \
            service_internal_argv[0] = &arg1; \
            service_internal_argv[1] = &arg2; \
            service_internal_argv[2] = &arg3; \
            service_internal_argv[3] = &arg4; \
            service_internal_argv[4] = &arg5; \
            ((struct service_t*)service)->exec(ret_value, service_internal_argv); \
        } while(0)

#define SERVICE_INTERNAL_GET_AND_CHECK(service_name) \
        struct service_t* service_internal_service = service_get(service_name); \
        if(!service_internal_service) \
            llog(LOG_WARNING, NULL, 3, "Service \"", service_name, "\" does not exist"); \
        else

#define SERVICE_CALL_NAME0(service_name, ret_value) do { \
            SERVICE_INTERNAL_GET_AND_CHECK(service_name) \
            SERVICE_CALL0(service_internal_service, ret_value); \
        } while(0)
#define SERVICE_CALL_NAME1(service_name, ret_value, arg1) do { \
            SERVICE_INTERNAL_GET_AND_CHECK(service_name) \
            SERVICE_CALL1(service_internal_service, ret_value, arg1); \
        } while(0)
#define SERVICE_CALL_NAME2(service_name, ret_value, arg1, arg2) do { \
            SERVICE_INTERNAL_GET_AND_CHECK(service_name) \
            SERVICE_CALL2(service_internal_service, ret_value, arg1, arg2); \
        } while(0)

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
#endif

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#define SERVICE_REGISTER0(plugin, service_name, callback, ret_type) do { \
            service_register(plugin, service_name, callback, STRINGIFY(ret_type), 0, NULL); \
        } while(0)
#define SERVICE_REGISTER1(plugin, service_name, callback, ret_type, arg1) do { \
            const char* ret = STRINGIFY(ret_type); \
            const char* argv[] = {STRINGIFY(arg1)}; \
            service_register(plugin, service_name, callback, ret, 1, argv); \
        } while(0)
#define SERVICE_REGISTER2(plugin, service_name, callback, ret_type, arg1, arg2) do { \
            const char* ret = STRINGIFY(ret_type); \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2)}; \
            service_register(plugin, service_name, callback, ret, 2, argv); \
        } while(0)
#define SERVICE_REGISTER3(plugin, service_name, callback, ret_type, arg1, arg2, arg3) do { \
            const char* ret = STRINGIFY(ret_type); \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2), STRINGIFY(arg3)}; \
            service_register(plugin, service_name, callback, ret, 3, argv); \
        } while(0)
#define SERVICE_REGISTER4(plugin, service_name, callback, ret_type, arg1, arg2, arg3, arg4) do { \
            const char* ret = STRINGIFY(ret_type); \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2), STRINGIFY(arg3), STRINGIFY(arg4)}; \
            service_register(plugin, service_name, callback, ret, 4, argv); \
        } while(0)
#define SERVICE_REGISTER5(plugin, service_name, callback, ret_type, arg1, arg2, arg3, arg4, arg5) do { \
            const char* ret = STRINGIFY(ret_type); \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2), STRINGIFY(arg3), STRINGIFY(arg4), STRINGIFY(arg5)}; \
            service_register(plugin, service_name, callback, ret, 5, argv); \
        } while(0)
#define SERVICE_REGISTER6(plugin, service_name, callback, ret_type, arg1, arg2, arg3, arg4, arg5, arg6) do { \
            const char* ret = STRINGIFY(ret_type); \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2), STRINGIFY(arg3), STRINGIFY(arg4), STRINGIFY(arg5), STRINGIFY(arg6)}; \
            service_register(plugin, service_name, callback, ret, 6, argv); \
        } while(0)

struct service_t
{
    char* name;
    uint32_t argc;
    const char* ret_type;
    const char** argv_type;
    service_callback_func exec;
};

#endif /* LIGHTSHIP_UTIL_SERVICE_H */
