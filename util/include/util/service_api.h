#ifndef LIGHTSHIP_UTIL_SERVICE_H
#define LIGHTSHIP_UTIL_SERVICE_H

#include "util/pstdint.h"
#include "util/config.h"

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#define SERVICE_REGISTER0(plugin, service_name, ret_type, callback) do { \
            SERVICE_STATIC_SIGNATURE_CHECK0(service_name, ret_type) \
            service_register_(plugin, service_name, STRINGIFY(ret_type), 0, NULL); \
        } while(0)
#define SERVICE_REGISTER1(plugin, service_name, callback, ret_type, arg1) do { \
            const char* ret = STRINGIFY(ret_type); \
            const char* argv[] = {STRINGIFY(arg1)}; \
            service_register_(plugin, service_name, (intptr_t)callback, ret, 1, argv); \
        } while(0)
#define SERVICE_REGISTER2(plugin, service_name, callback, ret_type, arg1, arg2) do { \
            const char* ret = STRINGIFY(ret_type); \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2)}; \
            service_register_(plugin, service_name, (intptr_t)callback, ret, 2, argv); \
        } while(0)
#define SERVICE_REGISTER3(plugin, service_name, callback, ret_type, arg1, arg2, arg3) do { \
            const char* ret = STRINGIFY(ret_type); \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2), STRINGIFY(arg3)}; \
            service_register_(plugin, service_name, (intptr_t)callback, ret, 3, argv); \
        } while(0)
#define SERVICE_REGISTER4(plugin, service_name, callback, ret_type, arg1, arg2, arg3, arg4) do { \
            const char* ret = STRINGIFY(ret_type); \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2), STRINGIFY(arg3), STRINGIFY(arg4)}; \
            service_register_(plugin, service_name, (intptr_t)callback, ret, 4, argv); \
        } while(0)

#define SERVICE_GET(ptr, name) \
        ptr = service_get(name)

#define SERVICE_GET_WITH_TYPECHECK0(ptr, service_name, ret_type) do { \
            const char* ret = STRINGIFY(ret_type); \
            ptr = service_get_with_typecheck(service_name, ret_type, 0, NULL); \
        } while(0)
#define SERVICE_GET_WITH_TYPECHECK1(ptr, service_name, ret_type, arg1) do { \
            const char* ret = STRINGIFY(ret_type); \
            const char** argv[] = {STRINGIFY(arg1)}; \
            ptr = service_get_with_typecheck(service_name, ret_type, 1, argv); \
        } while(0)
#define SERVICE_GET_WITH_TYPECHECK2(ptr, service_name, ret_type, arg1, arg2) do { \
            const char* ret = STRINGIFY(ret_type); \
            const char** argv[] = {STRINGIFY(arg1), STRINGIFY(arg2)}; \
            ptr = service_get_with_typecheck(service_name, ret_type, 2, argv); \
        } while(0)

struct service_callback_signature_t
{
    intptr_t exec;
    const char* ret_type;
    const char** argv;
    int argc;
};

struct service_t
{
    char* name;
    intptr_t exec;
    struct service_callback_signature_t cb;
};

#endif /* LIGHTSHIP_UTIL_SERVICE_H */
