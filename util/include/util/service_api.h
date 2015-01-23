#ifndef LIGHTSHIP_UTIL_SERVICE_H
#define LIGHTSHIP_UTIL_SERVICE_H

#include "util/pstdint.h"
#include "util/config.h"

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#define SERVICE(ret_type, callback) \
    typedef ret_type (*name)(void); \
    char* service_##callback_ret_type = #ret_type;

#define SERVICE_REGISTER0(plugin, service_name, ret_type, callback) \
    { \
        service_register_(plugin, service_name, STRINGIFY(ret_type), NULL); \
    }
#define SERVICE_REGISTER1(plugin, service_name, ret_type, callback, arg1) \
    { \
        service_register_(plugin, service_name, STRINGIFY(ret_type), {STRINGIFY(arg1)}); \
    }
#define SERVICE_REGISTER2(plugin, service_name, callback, ret_type, arg1, arg2) \
    { \
        const char* ret = STRINGIFY(ret_type); \
        const char* args[] = {STRINGIFY(arg1), STRINGIFY(arg2)}; \
        service_register_(plugin, service_name, (intptr_t)callback, ret, 2, args); \
    }

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
