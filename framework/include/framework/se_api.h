#ifndef FRAMEWORK_SERVICE_EVENT_API_H
#define FRAMEWORK_SERVICE_EVENT_API_H

#include "framework/config.h"
#include "framework/log.h"
#include "framework/plugin.h"
#include "util/dynamic_call.h"
#include "util/unordered_vector.h"
#include "util/pstdint.h"

struct plugin_t;
struct service_t;
struct event_t;

typedef void (*service_func)(struct service_t* service, void* ret, const void** argv);
typedef void (*event_callback_func)(struct event_t* event, const void** argv);

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

#define EVENT_LISTENER(evt_name) \
        void evt_name(struct event_t* event, const void** argv)

/* ------------------------------------------------------------------------- */
/*
 * In debug mode, we want to print the stack trace if an event object
 * is NULL. In release mode, we can skip the stack trace and skip the check
 * for event being NULL.
 */
/* ------------------------------------------------------------------------- */
#ifdef _DEBUG
#   include "util/backtrace.h"
#   include <stdlib.h>

/* this is the check for whether the event or service object is NULL or not */
#   define IF_OBJECT_VALID(obj)                                             \
        if((obj)) {

/*
 * This is the closure of the IF_OBJECT_VALID condition, which will print
 * the stacktrace if it enters the else condition.
 */
#   define ELSE_REPORT_FAILURE(msg)                                         \
        } else {                                                            \
            int size, i;                                                    \
            char** backtrace = get_backtrace(&size);                        \
            llog(LOG_ERROR, NULL, NULL, 1, msg);                            \
            for(i = 0; i != size; ++i)                                      \
                llog(LOG_ERROR, NULL, NULL, 1, backtrace[i]);               \
            if(backtrace) free(backtrace);                                  \
        }

#else  /* _DEBUG */
#   define ELSE_REPORT_FAILURE(msg)
#   define IF_OBJECT_VALID(event)
#endif /* _DEBUG */

/* used to iterate over the listeners of an event */
#define EVENT_ITERATE_LISTENERS_BEGIN(event)                                \
            { UNORDERED_VECTOR_FOR_EACH(&(event)->listeners,                \
                                        struct event_listener_t,            \
                                        listener)                           \
            {
#define EVENT_ITERATE_LISTENERS_END                                         \
            }}

#define GEN_ARGV_ON_STACK1(argv, arg1)                                      \
        const void* argv[1];                                                \
        argv[0] = &arg1;
#define GEN_ARGV_ON_STACK2(argv, arg1, arg2)                                \
        const void* argv[2];                                                \
        argv[0] = &arg1;                                                    \
        argv[1] = &arg2;
#define GEN_ARGV_ON_STACK3(argv, arg1, arg2, arg3)                          \
        const void* argv[3];                                                \
        argv[0] = &arg1;                                                    \
        argv[1] = &arg2;                                                    \
        argv[2] = &arg3;
#define GEN_ARGV_ON_STACK4(argv, arg1, arg2, arg3, arg4)                    \
        const void* argv[4];                                                \
        argv[0] = &arg1;                                                    \
        argv[1] = &arg2;                                                    \
        argv[2] = &arg3;                                                    \
        argv[3] = &arg4;
#define GEN_ARGV_ON_STACK5(argv, arg1, arg2, arg3, arg4, arg5)              \
        const void* argv[5];                                                \
        argv[0] = &arg1;                                                    \
        argv[1] = &arg2;                                                    \
        argv[2] = &arg3;                                                    \
        argv[3] = &arg4;                                                    \
        argv[4] = &arg5;
#define GEN_ARGV_ON_STACK6(argv, arg1, arg2, arg3, arg4, arg5, arg6)        \
        const void* argv[6];                                                \
        argv[0] = &arg1;                                                    \
        argv[1] = &arg2;                                                    \
        argv[2] = &arg3;                                                    \
        argv[3] = &arg4;                                                    \
        argv[4] = &arg5;                                                    \
        argv[5] = &arg6;

#define EVENT_FAIL_MSG "Cannot fire event for it is NULL"

#define EVENT_FIRE0(event) do {                                             \
            IF_OBJECT_VALID(event)                                          \
                EVENT_ITERATE_LISTENERS_BEGIN(event)                        \
                    listener->exec(event, NULL);                            \
                EVENT_ITERATE_LISTENERS_END                                 \
            ELSE_REPORT_FAILURE(EVENT_FAIL_MSG)                             \
        } while(0)
#define EVENT_FIRE1(event, arg1) do {                                       \
            IF_OBJECT_VALID(event)                                          \
                GEN_ARGV_ON_STACK1(event_internal_argv, arg1)               \
                EVENT_ITERATE_LISTENERS_BEGIN(event)                        \
                    listener->exec(event, event_internal_argv);             \
                EVENT_ITERATE_LISTENERS_END                                 \
            ELSE_REPORT_FAILURE(EVENT_FAIL_MSG)                             \
        } while(0)
#define EVENT_FIRE2(event, arg1, arg2) do {                                 \
            IF_OBJECT_VALID(event)                                          \
                GEN_ARGV_ON_STACK2(event_internal_argv, arg1, arg2)         \
                EVENT_ITERATE_LISTENERS_BEGIN(event)                        \
                    listener->exec(event, event_internal_argv);             \
                EVENT_ITERATE_LISTENERS_END                                 \
            ELSE_REPORT_FAILURE(EVENT_FAIL_MSG)                             \
        } while(0)
#define EVENT_FIRE3(event, arg1, arg2, arg3) do {                           \
            IF_OBJECT_VALID(event)                                          \
                GEN_ARGV_ON_STACK3(event_internal_argv, arg1, arg2, arg3)   \
                EVENT_ITERATE_LISTENERS_BEGIN(event)                        \
                    listener->exec(event, event_internal_argv);             \
                EVENT_ITERATE_LISTENERS_END                                 \
            ELSE_REPORT_FAILURE(EVENT_FAIL_MSG)                             \
        } while(0)
#define EVENT_FIRE4(event, arg1, arg2, arg3, arg4) do {                     \
            IF_OBJECT_VALID(event)                                          \
                GEN_ARGV_ON_STACK4(event_internal_argv, arg1, arg2, arg3,   \
                                   arg4)                                    \
                EVENT_ITERATE_LISTENERS_BEGIN(event)                        \
                    listener->exec(event, event_internal_argv);             \
                EVENT_ITERATE_LISTENERS_END                                 \
            ELSE_REPORT_FAILURE(EVENT_FAIL_MSG)                             \
        } while(0)
#define EVENT_FIRE5(event, arg1, arg2, arg3, arg4, arg5) do {               \
            IF_OBJECT_VALID(event)                                          \
                GEN_ARGV_ON_STACK5(event_internal_argv, arg1, arg2, arg3,   \
                                   arg4, arg5)                              \
                EVENT_ITERATE_LISTENERS_BEGIN(event)                        \
                    listener->exec(event, event_internal_argv);             \
                EVENT_ITERATE_LISTENERS_END                                 \
            ELSE_REPORT_FAILURE(EVENT_FAIL_MSG)                             \
        } while(0)
#define EVENT_FIRE6(event, arg1, arg2, arg3, arg4, arg5, arg6) do {         \
            IF_OBJECT_VALID(event)                                          \
                GEN_ARGV_ON_STACK6(event_internal_argv, arg1, arg2, arg3,   \
                                   arg4, arg5, arg6)                        \
                EVENT_ITERATE_LISTENERS_BEGIN(event)                        \
                    listener->exec(event, event_internal_argv);             \
                EVENT_ITERATE_LISTENERS_END                                 \
            ELSE_REPORT_FAILURE(EVENT_FAIL_MSG)                             \
        } while(0)

#define SERVICE_FAIL_MSG "Cannot call service for it is NULL"

#define SERVICE_CALL0(service, ret_value) do {                              \
            IF_OBJECT_VALID(service)                                        \
                (service)->exec(service, ret_value, NULL);                  \
            ELSE_REPORT_FAILURE(service)                                    \
            } while(0)
#define SERVICE_CALL1(service, ret_value, arg1) do {                        \
            IF_OBJECT_VALID(service)                                        \
                GEN_ARGV_ON_STACK1(service_internal_argv, arg1)             \
                (service)->exec(service, ret_value, service_internal_argv); \
            ELSE_REPORT_FAILURE(service)                                    \
        } while(0)
#define SERVICE_CALL2(service, ret_value, arg1, arg2) do {                  \
            IF_OBJECT_VALID(service)                                        \
                GEN_ARGV_ON_STACK2(service_internal_argv, arg1, arg2)       \
                (service)->exec(service, ret_value, service_internal_argv); \
            ELSE_REPORT_FAILURE(service)                                    \
        } while(0)
#define SERVICE_CALL3(service, ret_value, arg1, arg2, arg3) do {            \
            IF_OBJECT_VALID(service)                                        \
                GEN_ARGV_ON_STACK3(service_internal_argv, arg1, arg2, arg3) \
                (service)->exec(service, ret_value, service_internal_argv); \
            ELSE_REPORT_FAILURE(service)                                    \
        } while(0)
#define SERVICE_CALL4(service, ret_value, arg1, arg2, arg3, arg4) do {      \
            IF_OBJECT_VALID(service)                                        \
                GEN_ARGV_ON_STACK4(service_internal_argv, arg1, arg2, arg3, \
                                   arg4)                                    \
                (service)->exec(service, ret_value, service_internal_argv); \
            ELSE_REPORT_FAILURE(service)                                    \
        } while(0)
#define SERVICE_CALL5(service, ret_value, arg1, arg2, arg3, arg4, arg5)     \
        do {                                                                \
            IF_OBJECT_VALID(service)                                        \
                GEN_ARGV_ON_STACK5(service_internal_argv, arg1, arg2, arg3, \
                                   arg4, arg5)                              \
                (service)->exec(service, ret_value, service_internal_argv); \
            ELSE_REPORT_FAILURE(service)                                    \
        } while(0)
#define SERVICE_CALL6(service, ret_value, arg1, arg2, arg3, arg4, arg5,     \
                      arg6) do {                                            \
            IF_OBJECT_VALID(service)                                        \
                GEN_ARGV_ON_STACK6(service_internal_argv, arg1, arg2, arg3, \
                                   arg4, arg5, arg6)                        \
                (service)->exec(service, ret_value, service_internal_argv); \
            ELSE_REPORT_FAILURE(service)                                    \
        } while(0)

#define SERVICE_INTERNAL_GET_AND_CHECK(game, directory)                     \
        struct service_t* service_internal_service = service_get(game,      \
                                                        directory);         \
        if(!service_internal_service)                                       \
            llog(LOG_WARNING, game, NULL, 3, "Service \"", directory,       \
                        "\" does not exist");                               \
        else

#define SERVICE_CALL_NAME0(game, directory, ret_value) do {                 \
            SERVICE_INTERNAL_GET_AND_CHECK(game, directory)                 \
            SERVICE_CALL0(service_internal_service, ret_value);             \
        } while(0)
#define SERVICE_CALL_NAME1(game, directory, ret_value, arg1) do {           \
            SERVICE_INTERNAL_GET_AND_CHECK(game, directory)                 \
            SERVICE_CALL1(service_internal_service, ret_value, arg1);       \
        } while(0)
#define SERVICE_CALL_NAME2(game, directory, ret_value, arg1, arg2) do {     \
            SERVICE_INTERNAL_GET_AND_CHECK(game, directory)                 \
            SERVICE_CALL2(service_internal_service, ret_value, arg1, arg2); \
        } while(0)
#define SERVICE_CALL_NAME3(game, directory, ret_value, arg1, arg2, arg3)    \
        do {                                                                \
            SERVICE_INTERNAL_GET_AND_CHECK(game, directory)                 \
            SERVICE_CALL3(service_internal_service, ret_value, arg1, arg2,  \
                          arg3);                                            \
        } while(0)
#define SERVICE_CALL_NAME4(game, directory, ret_value, arg1, arg2, arg3,    \
                           arg4) do {                                       \
            SERVICE_INTERNAL_GET_AND_CHECK(game, directory)                 \
            SERVICE_CALL4(service_internal_service, ret_value, arg1, arg2,  \
                          arg3, arg4);                                      \
        } while(0)
#define SERVICE_CALL_NAME5(game, directory, ret_value, arg1, arg2, arg3,    \
                           arg4, arg5) do {                                 \
            SERVICE_INTERNAL_GET_AND_CHECK(game, directory)                 \
            SERVICE_CALL5(service_internal_service, ret_value, arg1, arg2,  \
                          arg3, arg4, arg5);                                \
        } while(0)
#define SERVICE_CALL_NAME6(game, directory, ret_value, arg1, arg2, arg3,    \
                           arg4, arg5, arg6) do {                           \
            SERVICE_INTERNAL_GET_AND_CHECK(game, directory)                 \
            SERVICE_CALL6(service_internal_service, ret_value, arg1, arg2,  \
                          arg3, arg4, arg5, arg6);                          \
        } while(0)

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#define EVENT_CREATE0(plugin, evt, directory) do {                          \
            struct type_info_t* t;                                          \
            t = dynamic_call_create_type_info("void", 0, NULL);             \
            if(!t) { evt = NULL; break; }                                   \
            if(!(evt = event_create(plugin, directory, t)))                 \
                dynamic_call_destroy_type_info(t);                          \
        } while(0)
#define EVENT_CREATE1(plugin, evt, directory, arg1) do {                    \
            struct type_info_t* t;                                          \
            const char* argv[] = {STRINGIFY(arg1)};                         \
            t = dynamic_call_create_type_info("void", 1, argv);             \
            if(!t) { evt = NULL; break; }                                   \
            if(!(evt = event_create(plugin, directory, t)))                 \
                dynamic_call_destroy_type_info(t);                          \
        } while(0)
#define EVENT_CREATE2(plugin, evt, directory, arg1, arg2) do {              \
            struct type_info_t* t;                                          \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2)};        \
            t = dynamic_call_create_type_info("void", 2, argv);             \
            if(!t) { evt = NULL; break; }                                   \
            if(!(evt = event_create(plugin, directory, t)))                 \
                dynamic_call_destroy_type_info(t);                          \
        } while(0)
#define EVENT_CREATE3(plugin, evt, directory, arg1, arg2, arg3) do {        \
            struct type_info_t* t;                                          \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2),         \
                                  STRINGIFY(arg3)};                         \
            t = dynamic_call_create_type_info("void", 3, argv);             \
            if(!t) { evt = NULL; break; }                                   \
            if(!(evt = event_create(plugin, directory, t)))                 \
                dynamic_call_destroy_type_info(t);                          \
        } while(0)
#define EVENT_CREATE4(plugin, evt, directory, arg1, arg2, arg3, arg4) do {  \
            struct type_info_t* t;                                          \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2),         \
                                  STRINGIFY(arg3), STRINGIFY(arg4)};        \
            t = dynamic_call_create_type_info("void", 4, argv);             \
            if(!t) { evt = NULL; break; }                                   \
            if(!(evt = event_create(plugin, directory, t)))                 \
                dynamic_call_destroy_type_info(t);                          \
        } while(0)
#define EVENT_CREATE5(plugin, evt, directory, arg1, arg2, arg3, arg4, arg5) \
        do {                                                                \
            struct type_info_t* t;                                          \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2),         \
                                  STRINGIFY(arg3), STRINGIFY(arg4),         \
                                  STRINGIFY(arg5)};                         \
            t = dynamic_call_create_type_info("void", 5, argv);             \
            if(!t) { evt = NULL; break; }                                   \
            if(!(evt = event_create(plugin, directory, t)))                 \
                dynamic_call_destroy_type_info(t);                          \
        } while(0)
#define EVENT_CREATE6(plugin, evt, directory, arg1, arg2, arg3, arg4, arg5, \
        arg6) do {                                                          \
            struct type_info_t* t;                                          \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2),         \
                                  STRINGIFY(arg3), STRINGIFY(arg4),         \
                                  STRINGIFY(arg5), STRINGIFY(arg6)};        \
            t = dynamic_call_create_type_info("void", 6, argv);             \
            if(!t) { evt = NULL; break; }                                   \
            if(!(evt = event_create(plugin, directory, t)))                 \
                dynamic_call_destroy_type_info(t);                          \
        } while(0)


#define SERVICE_CREATE0(plugin, serv, directory, callback, ret_type)        \
        do {                                                                \
            struct type_info_t* t;                                          \
            t = dynamic_call_create_type_info(STRINGIFY(ret_type),          \
                                              0,                            \
                                              NULL);                        \
            if(!t) { serv = NULL; break; }                                  \
            if(!(serv = service_create(plugin, directory, callback, t)))    \
                dynamic_call_destroy_type_info(t);                          \
        } while(0)
#define SERVICE_CREATE1(plugin, serv, directory, callback, ret_type,        \
                        arg1) do {                                          \
            struct type_info_t* t;                                          \
            const char* ret = STRINGIFY(ret_type);                          \
            const char* argv[] = {STRINGIFY(arg1)};                         \
            t = dynamic_call_create_type_info(ret, 1, argv);                \
            if(!t) { serv = NULL; break; }                                  \
            if(!(serv = service_create(plugin, directory, callback, t)))    \
                dynamic_call_destroy_type_info(t);                          \
        } while(0)
#define SERVICE_CREATE2(plugin, serv, directory, callback, ret_type,        \
                        arg1, arg2) do {                                    \
            struct type_info_t* t;                                          \
            const char* ret = STRINGIFY(ret_type);                          \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2)};        \
            t = dynamic_call_create_type_info(ret, 2, argv);                \
            if(!t) { ret = NULL; break; }                                   \
            if(!(serv = service_create(plugin, directory, callback, t)))    \
                dynamic_call_destroy_type_info(t);                          \
        } while(0)
#define SERVICE_CREATE3(plugin, serv, directory, callback, ret_type,        \
                        arg1, arg2, arg3) do {                              \
            struct type_info_t* t;                                          \
            const char* ret = STRINGIFY(ret_type);                          \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2),         \
                                  STRINGIFY(arg3)};                         \
            t = dynamic_call_create_type_info(ret, 3, argv);                \
            if(!t) { serv = NULL; break; }                                  \
            if(!(serv = service_create(plugin, directory, callback, t)))    \
                dynamic_call_destroy_type_info(t);                          \
        } while(0)
#define SERVICE_CREATE4(plugin, serv, directory, callback, ret_type,        \
                        arg1, arg2, arg3, arg4) do {                        \
            struct type_info_t* t;                                          \
            const char* ret = STRINGIFY(ret_type);                          \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2),         \
                                  STRINGIFY(arg3), STRINGIFY(arg4)};        \
            t = dynamic_call_create_type_info(ret, 4, argv);                \
            if(!t) { serv = NULL; break; }                                  \
            if(!(serv = service_create(plugin, directory, callback, t)))    \
                dynamic_call_destroy_type_info(t);                          \
        } while(0)
#define SERVICE_CREATE5(plugin, serv, directory, callback, ret_type,        \
                        arg1, arg2, arg3, arg4, arg5) do {                  \
            struct type_info_t* t;                                          \
            const char* ret = STRINGIFY(ret_type);                          \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2),         \
                                  STRINGIFY(arg3), STRINGIFY(arg4),         \
                                  STRINGIFY(arg5)};                         \
            t = dynamic_call_create_type_info(ret, 5, argv);                \
            if(!t) { serv = NULL; break; }                                  \
            if(!(serv = service_create(plugin, directory, callback, t)))    \
                dynamic_call_destroy_type_info(t);                          \
        } while(0)
#define SERVICE_CREATE6(plugin, serv, directory, callback, ret_type,        \
                        arg1, arg2, arg3, arg4, arg5, arg6) do {            \
            struct type_info_t* t;                                          \
            const char* ret = STRINGIFY(ret_type);                          \
            const char* argv[] = {STRINGIFY(arg1), STRINGIFY(arg2),         \
                                  STRINGIFY(arg3), STRINGIFY(arg4),         \
                                  STRINGIFY(arg5), STRINGIFY(arg6)};        \
            t = dynamic_call_create_type_info(ret, 6, argv);                \
            if(!t) { serv = NULL; break; }                                  \
            if(!(serv = service_create(plugin, directory, callback, t)))    \
                dynamic_call_destroy_type_info(t);                          \
        } while(0)

/* ------------------------------------------------------------------------- *
 * The following is the event listener and dispatch system implemented       *
 * partially in macros. We have no access to variadic macros so it may       *
 * appear a little bloated.                                                  *
 * ------------------------------------------------------------------------- */

/*!
 * @brief Helper macro for creating listener functions with up to 4 receiving
 * function parameters.
 *
 * You can use this for function prototypes as well as function definitions.
 * Example:
 * ```
 * EVENT_LISTENER1(on_event, int arg);
 *
 * ...
 *
 * EVENT_LISTENER1(on_event, int arg)
 * {
 * }
 * ```
 * @param[in] name The name to give the event listener. Usually something like
 * *on_event*.
 * @param[in] arg... The arguments to receive from the event.
 * @note The event listener must have the same signature as the fired event.
 */
#define EVENT_LISTENER0(name)                                               \
    void name(const struct event_t* event)
#define EVENT_LISTENER1(name, arg)                                          \
    void name(const struct event_t* event, arg)
#define EVENT_LISTENER2(name, arg1, arg2)                                   \
    void name(const struct event_t* event, arg1, arg2)
#define EVENT_LISTENER3(name, arg1, arg2, arg3)                             \
    void name(const struct event_t* event, arg1, arg2, arg3)
#define EVENT_LISTENER4(name, arg1, arg2, arg3, arg4)                       \
    void name(const struct event_t* event, arg1, arg2, arg3, arg4)

/*!
 * @brief Generates an event listener function signature typedef.
 */
#define EVENT_LISTENER_TYPEDEF0(name)                                       \
    typedef void (*name)(const struct event_t*);
#define EVENT_LISTENER_TYPEDEF1(name, arg)                                  \
    typedef void (*name)(const struct event_t*, arg);
#define EVENT_LISTENER_TYPEDEF2(name, arg1, arg2)                           \
    typedef void (*name)(const struct event_t*, arg1, arg2);
#define EVENT_LISTENER_TYPEDEF3(name, arg1, arg2, arg3)                     \
    typedef void (*name)(const struct event_t*, arg1, arg2, arg3);
#define EVENT_LISTENER_TYPEDEF4(name, arg1, arg2, arg3, arg4)               \
    typedef void (*name)(const struct event_t*, arg1, arg2, arg3, arg4);

#endif /* FRAMEWORK_SERVICE_EVENT_API_H */
