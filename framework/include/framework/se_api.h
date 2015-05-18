#ifndef FRAMEWORK_SERVICE_EVENT_API_H
#define FRAMEWORK_SERVICE_EVENT_API_H

#include "util/pstdint.h"
#include "util/dynamic_call.h"
#include "framework/config.h"

struct plugin_t;
struct service_t;
struct event_t;

typedef void (*service_func)(struct service_t* service, void* ret, const void** argv);
typedef void (*event_func)(struct event_t* event, const void** argv);

/*!
 * @brief Helper macro for defining a service function.
 *
 * This can be used in the header file to create a function prototype as well
 * as in the source file to define the function body.
 *
 * The resulting function signature defines 3 arguments:
 *   - service: The service object that called the service function. This can
 *              be used to extract the game object (service->plugin->game), retrieve
 *              type information about the arguments, or otherwise help
 *              identify information about the service.
 *   - ret    : A void* pointing to a location where a return value can be
 *              written to. See the helper macro RETURN() for more
 *              information on returning values from service functions.
 *   - argv   : An argument vector of void** pointing to the memory locations
 *              where argument types can be read from. See the helper macros
 *              EXTRACT_ARGUMENT() and EXTRACT_ARGUMENT_PTR()
 *              for more information on extracting arguments from service
 *              functions.
 * @param func_name The name of the service function.
 */
#define SERVICE(func_name) \
        void func_name(struct service_t* service, void* ret, const void** argv)

#define EVENT(evt_name) \
        void evt_name(struct event_t* event, const void** argv)


#define SERVICE_CALL0(service, ret_value) do {                                                  \
            (service)->exec(service, ret_value, NULL);                                          \
        } while(0)
#define SERVICE_CALL1(service, ret_value, arg1) do {                                            \
            const void* service_internal_argv[1];                                               \
            service_internal_argv[0] = &arg1;                                                   \
            (service)->exec(service, ret_value, service_internal_argv);                         \
        } while(0)
#define SERVICE_CALL2(service, ret_value, arg1, arg2) do {                                      \
            const void* service_internal_argv[2];                                               \
            service_internal_argv[0] = &arg1;                                                   \
            service_internal_argv[1] = &arg2;                                                   \
            (service)->exec(service, ret_value, service_internal_argv);                         \
        } while(0)
#define SERVICE_CALL3(service, ret_value, arg1, arg2, arg3) do {                                \
            const void* service_internal_argv[3];                                               \
            service_internal_argv[0] = &arg1;                                                   \
            service_internal_argv[1] = &arg2;                                                   \
            service_internal_argv[2] = &arg3;                                                   \
            (service)->exec(service, ret_value, service_internal_argv);                         \
        } while(0)
#define SERVICE_CALL4(service, ret_value, arg1, arg2, arg3, arg4) do {                          \
            const void* service_internal_argv[4];                                               \
            service_internal_argv[0] = &arg1;                                                   \
            service_internal_argv[1] = &arg2;                                                   \
            service_internal_argv[2] = &arg3;                                                   \
            service_internal_argv[3] = &arg4;                                                   \
            (service)->exec(service, ret_value, service_internal_argv);                         \
        } while(0)
#define SERVICE_CALL5(service, ret_value, arg1, arg2, arg3, arg4, arg5) do {                    \
            const void* service_internal_argv[5];                                               \
            service_internal_argv[0] = &arg1;                                                   \
            service_internal_argv[1] = &arg2;                                                   \
            service_internal_argv[2] = &arg3;                                                   \
            service_internal_argv[3] = &arg4;                                                   \
            service_internal_argv[4] = &arg5;                                                   \
            (service)->exec(service, ret_value, service_internal_argv);                         \
        } while(0)
#define SERVICE_CALL6(service, ret_value, arg1, arg2, arg3, arg4, arg5, arg6) do {              \
            const void* service_internal_argv[6];                                               \
            service_internal_argv[0] = &arg1;                                                   \
            service_internal_argv[1] = &arg2;                                                   \
            service_internal_argv[2] = &arg3;                                                   \
            service_internal_argv[3] = &arg4;                                                   \
            service_internal_argv[4] = &arg5;                                                   \
            service_internal_argv[5] = &arg6;                                                   \
            (service)->exec(service, ret_value, service_internal_argv);                         \
        } while(0)

#define SERVICE_INTERNAL_GET_AND_CHECK(game, service_name)                                      \
        struct service_t* service_internal_service = service_get(game, service_name);           \
        if(!service_internal_service)                                                           \
            llog(LOG_WARNING, game, NULL, 3, "Service \"", service_name, "\" does not exist");  \
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

#define SERVICE_CREATE0(plugin, assign, service_name, callback, ret_type) do {                          \
            struct type_info_t* t = dynamic_call_create_type_info(STRINGIFY(ret_type),                  \
                                                                  0,                                    \
                                                                  NULL);                                \
            if(!t) { assign = NULL; break; }                                                            \
            assign = service_create(plugin, service_name, callback, t);                                 \
        } while(0)
#define SERVICE_CREATE1(plugin, assign, service_name, callback, ret_type, arg1) do {                    \
            const char* ret = STRINGIFY(ret_type);                                                      \
            const char* argv[] = {STRINGIFY(arg1)};                                                     \
            struct type_info_t* t = dynamic_call_create_type_info(ret, 1, argv);                        \
            if(!t) { assign = NULL; break; }                                                            \
            assign = service_create(plugin, service_name, callback, t);                                 \
        } while(0)
#define SERVICE_CREATE2(plugin, assign, service_name, callback, ret_type, arg1, arg2) do {              \
            const char* ret = STRINGIFY(ret_type);                                                      \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2)};                                    \
            struct type_info_t* t = dynamic_call_create_type_info(ret, 2, argv);                        \
            if(!t) { assign = NULL; break; }                                                            \
            assign = service_create(plugin, service_name, callback, t);                                 \
        } while(0)
#define SERVICE_CREATE3(plugin, assign, service_name, callback, ret_type, arg1, arg2, arg3) do {        \
            const char* ret = STRINGIFY(ret_type);                                                      \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2), STRINGIFY(arg3)};                   \
            struct type_info_t* t = dynamic_call_create_type_info(ret, 3, argv);                        \
            if(!t) { assign = NULL; break; }                                                            \
            assign = service_create(plugin, service_name, callback, t);                                 \
        } while(0)
#define SERVICE_CREATE4(plugin, assign, service_name, callback, ret_type, arg1, arg2, arg3, arg4) do {  \
            const char* ret = STRINGIFY(ret_type);                                                      \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2), STRINGIFY(arg3), STRINGIFY(arg4)};  \
            struct type_info_t* t = dynamic_call_create_type_info(ret, 4, argv);                        \
            if(!t) { assign = NULL; break; }                                                            \
            assign = service_create(plugin, service_name, callback, t);                                 \
        } while(0)
#define SERVICE_CREATE5(plugin, assign, service_name, callback, ret_type, arg1, arg2, arg3, arg4, arg5) do {            \
            const char* ret = STRINGIFY(ret_type);                                                                      \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2), STRINGIFY(arg3), STRINGIFY(arg4), STRINGIFY(arg5)}; \
            struct type_info_t* t = dynamic_call_create_type_info(ret, 5, argv);                                        \
            if(!t) { assign = NULL; break; }                                                                            \
            assign = service_create(plugin, service_name, callback, t);                                                 \
        } while(0)
#define SERVICE_CREATE6(plugin, assign, service_name, callback, ret_type, arg1, arg2, arg3, arg4, arg5, arg6) do {      \
            const char* ret = STRINGIFY(ret_type);                                                                      \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2), STRINGIFY(arg3), STRINGIFY(arg4), STRINGIFY(arg5),  \
                                  STRINGIFY(arg6)};                                                                     \
            struct type_info_t* t = dynamic_call_create_type_info(ret, 6, argv);                                        \
            if(!t) { assign = NULL; break; }                                                                            \
            assign = service_create(plugin, service_name, callback, t);                                                 \
        } while(0)

struct service_t
{
    struct plugin_t* plugin;    /* reference to the plugin that owns this service */
    char* directory;
    service_func exec;
    struct type_info_t* type_info;
};

#endif /* FRAMEWORK_SERVICE_EVENT_API_H */
