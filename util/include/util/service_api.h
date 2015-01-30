#ifndef LIGHTSHIP_UTIL_SERVICE_H
#define LIGHTSHIP_UTIL_SERVICE_H

#include "util/pstdint.h"
#include "util/config.h"

struct service_t;
typedef void* (*service_callback_func)(const void** argv);

#define SERVICE(func_name) \
        void* func_name(const void** argv)

#define SERVICE_NO_RET void* service_internal_null

#define SERVICE_CALL0(service, ret_value) do { \
            ret_value = ((struct service_t*)service)->exec(NULL); \
        } while(0)
#define SERVICE_CALL1(service, ret_value, arg1) do { \
            const void* service_internal_argv[1] = {&arg1}; \
            ret_value = ((struct service_t*)service)->exec(service_internal_argv); \
        } while(0)
#define SERVICE_CALL2(service, ret_value, arg1, arg2) do { \
            const void* service_internal_argv[2] = {&arg1, &arg2}; \
            ret_value = ((struct service_t*)service)->exec(service_internal_argv); \
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
    int argc;
    const char* ret_type;
    const char** argv_type;
    service_callback_func exec;
};

#endif /* LIGHTSHIP_UTIL_SERVICE_H */
