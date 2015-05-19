#ifndef FRAMEWORK_SERVICE_EVENT_API_H
#define FRAMEWORK_SERVICE_EVENT_API_H

#include "framework/config.h"
#include "framework/plugin.h"
#include "util/dynamic_call.h"
#include "util/unordered_vector.h"
#include "util/pstdint.h"

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
#define EVENT_LISTENER0(name) \
    void name(const struct event_t* event)
#define EVENT_LISTENER1(name, arg) \
    void name(const struct event_t* event, arg)
#define EVENT_LISTENER2(name, arg1, arg2) \
    void name(const struct event_t* event, arg1, arg2)
#define EVENT_LISTENER3(name, arg1, arg2, arg3) \
    void name(const struct event_t* event, arg1, arg2, arg3)
#define EVENT_LISTENER4(name, arg1, arg2, arg3, arg4) \
    void name(const struct event_t* event, arg1, arg2, arg3, arg4)

/*!
 * @brief Generates an event listener function signature typedef.
 */
#define EVENT_LISTENER_TYPEDEF0(name) \
    typedef void (*name)(const struct event_t*);
#define EVENT_LISTENER_TYPEDEF1(name, arg) \
    typedef void (*name)(const struct event_t*, arg);
#define EVENT_LISTENER_TYPEDEF2(name, arg1, arg2) \
    typedef void (*name)(const struct event_t*, arg1, arg2);
#define EVENT_LISTENER_TYPEDEF3(name, arg1, arg2, arg3) \
    typedef void (*name)(const struct event_t*, arg1, arg2, arg3);
#define EVENT_LISTENER_TYPEDEF4(name, arg1, arg2, arg3, arg4) \
    typedef void (*name)(const struct event_t*, arg1, arg2, arg3, arg4);

/*
 * In debug mode, we want to print the stack trace if an event object
 * is NULL. In release mode, we can skip the stack trace and skip the check
 * for event being NULL.
 */
#ifdef _DEBUG
#   include "util/backtrace.h"
#   include <stdlib.h>

/* this is the check for whether the event object is NULL or not */
#   define IF_EVENT_VALID(event) \
                if((event)) {

/*
 * This is the closure of the IF_EVENT_VALID condition, which will print
 * the stacktrace.
 */
#   define ELSE_REPORT_EVENT_FIRE_FAILURE(event)                                        \
                } else {                                                                \
                    int size, i;                                                        \
                    char** backtrace = get_backtrace(&size);                            \
                    llog(LOG_ERROR, (event)->plugin->game, (event)->plugin, 1, "Cannot fire event for it is NULL");\
                    llog_indent((event)->plugin->game, "Backtrace");                    \
                    for(i = 0; i != size; ++i)                                          \
                        llog(LOG_ERROR, (event)->plugin->game, NULL, 1, backtrace[i]);  \
                    if(backtrace) free(backtrace);                                      \
                    llog_unindent((event)->plugin->game);                               \
                }

#else  /* _DEBUG */
#   define ELSE_REPORT_EVENT_FIRE_FAILURE(event)
#   define IF_EVENT_VALID(event)
#endif /* _DEBUG */

/* used to iterate over the listeners of an event */
#define EVENT_ITERATE_LISTENERS_BEGIN(event)                                                    \
            UNORDERED_VECTOR_FOR_EACH(&(event)->listeners, struct event_listener_t, listener)   \
            {
#define EVENT_ITERATE_LISTENERS_END \
            }

/*
 * If we have access to TYPEOF, it is possible to enforce matching function
 * signatures somewhat within plugins. This is accomplished by creating a
 * function pointer and comparing it to a dummy function created when EVENT_C
 * and EVENT_H are called. This means that the arguments passed to EVENT_H,
 * EVENT_C, and EVENT_FIRE must match, or a compiler warning will be generated.
 * Only do this for debug builds.
 */
#if defined(TYPEOF) && defined(_DEBUG)
    /* these are the dummy functions generated in EVENT_H and EVENT_C */
#   define EVENT_GEN_DUMMY_FUNCTION_DECL0(event) \
            void event_internal_func_##event(const struct event_t* evt);
#   define EVENT_GEN_DUMMY_FUNCTION_DECL1(event, arg1_t) \
            void event_internal_func_##event(const struct event_t* evt, arg1_t arg1);
#   define EVENT_GEN_DUMMY_FUNCTION_DECL2(event, arg1_t, arg2_t) \
            void event_internal_func_##event(const struct event_t* evt, arg1_t arg1, arg2_t arg2);
#   define EVENT_GEN_DUMMY_FUNCTION_DECL3(event, arg1_t, arg2_t, arg3_t) \
            void event_internal_func_##event(const struct event_t* evt, arg1_t arg1, arg2_t arg2, arg3_t arg3);
#   define EVENT_GEN_DUMMY_FUNCTION_DECL4(event, arg1_t, arg2_t, arg3_t, arg4_t) \
            void event_internal_func_##event(const struct event_t* evt, arg1_t arg1, arg2_t arg2, arg3_t arg3, arg4_t arg4);
    /* function bodies of the dummy functions */
#   define EVENT_GEN_DUMMY_FUNCTION_DEF0(event) \
            void event_internal_func_##event(const struct event_t* evt) {}
#   define EVENT_GEN_DUMMY_FUNCTION_DEF1(event, arg1_t) \
            void event_internal_func_##event(const struct event_t* evt, arg1_t arg1) {}
#   define EVENT_GEN_DUMMY_FUNCTION_DEF2(event, arg1_t, arg2_t) \
            void event_internal_func_##event(const struct event_t* evt, arg1_t arg1, arg2_t arg2) {}
#   define EVENT_GEN_DUMMY_FUNCTION_DEF3(event, arg1_t, arg2_t, arg3_t) \
            void event_internal_func_##event(const struct event_t* evt, arg1_t arg1, arg2_t arg2, arg3_t arg3) {}
#   define EVENT_GEN_DUMMY_FUNCTION_DEF4(event, arg1_t, arg2_t, arg3_t, arg4_t) \
            void event_internal_func_##event(const struct event_t* evt, arg1_t arg1, arg2_t arg2, arg3_t arg3, arg4_t arg4) {}

    /* these are the comparison checks used in EVENT_FIRE */
#   define EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK0(event) do { \
            void (*func)(const struct event_t*); \
            (void)(func == event_internal_func_##event); } while(0);
#   define EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK1(event, arg1) do { \
            void (*func)(const struct event_t*, TYPEOF(arg1)); \
            (void)(func == event_internal_func_##event); } while(0);
#   define EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK2(event, arg1, arg2) do { \
            void (*func)(const struct event_t*, TYPEOF(arg1), TYPEOF(arg2)); \
            (void)(func == event_internal_func_##event); } while(0);
#   define EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK3(event, arg1, arg2, arg3) do { \
            void (*func)(const struct event_t*, TYPEOF(arg1), TYPEOF(arg2), TYPEOF(arg3)); \
            (void)(func == event_internal_func_##event); } while(0);
#   define EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK4(event, arg1, arg2, arg3, arg4) do { \
            void (*func)(const struct event_t*, TYPEOF(arg1), TYPEOF(arg2), TYPEOF(arg3), TYPEOF(arg4)); \
            (void)(func == event_internal_func_##event); } while(0);
#else
#   define EVENT_GEN_DUMMY_FUNCTION_DECL0(event)
#   define EVENT_GEN_DUMMY_FUNCTION_DECL1(event, arg1)
#   define EVENT_GEN_DUMMY_FUNCTION_DECL2(event, arg1, arg2)
#   define EVENT_GEN_DUMMY_FUNCTION_DECL3(event, arg1, arg2, arg3)
#   define EVENT_GEN_DUMMY_FUNCTION_DECL4(event, arg1, arg2, arg3, arg4)
#   define EVENT_GEN_DUMMY_FUNCTION_DEF0(event)
#   define EVENT_GEN_DUMMY_FUNCTION_DEF1(event, arg1)
#   define EVENT_GEN_DUMMY_FUNCTION_DEF2(event, arg1, arg2)
#   define EVENT_GEN_DUMMY_FUNCTION_DEF3(event, arg1, arg2, arg3)
#   define EVENT_GEN_DUMMY_FUNCTION_DEF4(event, arg1, arg2, arg3, arg4)
#   define EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK0(event)
#   define EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK1(event, arg1)
#   define EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK2(event, arg1, arg2)
#   define EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK3(event, arg1, arg2, arg3)
#   define EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK4(event, arg1, arg2, arg3, arg4)

#endif

/*!
 * @brief Fires the specified event with the specified arguments.
 * @param[in] event The event to fire. Should be declared with EVENT_C/EVENT_H
 * and be registered with the host program with event_register().
 * @param[in] args... The arguments to fire along with the event.
 */
#define EVENT_FIRE0(event)                          EVENT_FIRE_FROM_TEMP0(event, event)
#define EVENT_FIRE1(event, arg1)                    EVENT_FIRE_FROM_TEMP1(event, event, arg1)
#define EVENT_FIRE2(event, arg1, arg2)              EVENT_FIRE_FROM_TEMP2(event, event, arg1, arg2)
#define EVENT_FIRE3(event, arg1, arg2, arg3)        EVENT_FIRE_FROM_TEMP3(event, event, arg1, arg2, arg3)
#define EVENT_FIRE4(event, arg1, arg2, arg3, arg4)  EVENT_FIRE_FROM_TEMP4(event, event, arg1, arg2, arg3, arg4)

#define EVENT_FIRE_FROM_TEMP0(static_name, event) do {          \
            EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK0(static_name)  \
            IF_EVENT_VALID(event)                               \
                EVENT_ITERATE_LISTENERS_BEGIN(event)            \
                    EVENT_FIRE_IMPL0(event)                     \
                EVENT_ITERATE_LISTENERS_END                     \
            ELSE_REPORT_EVENT_FIRE_FAILURE(event) } while(0)
#define EVENT_FIRE_FROM_TEMP1(static_name, event, arg) do {         \
            EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK1(static_name, arg) \
            IF_EVENT_VALID(event)                                   \
                EVENT_ITERATE_LISTENERS_BEGIN(event)                \
                    EVENT_FIRE_IMPL1(event, arg)                    \
                EVENT_ITERATE_LISTENERS_END                         \
            ELSE_REPORT_EVENT_FIRE_FAILURE(event) } while(0)
#define EVENT_FIRE_FROM_TEMP2(static_name, event, arg1, arg2) do {          \
            EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK2(static_name, arg1, arg2)  \
            IF_EVENT_VALID(event)                                           \
                EVENT_ITERATE_LISTENERS_BEGIN(event)                        \
                    EVENT_FIRE_IMPL2(event, arg1, arg2)                     \
                EVENT_ITERATE_LISTENERS_END                                 \
            ELSE_REPORT_EVENT_FIRE_FAILURE(event) } while(0)
#define EVENT_FIRE_FROM_TEMP3(static_name, event, arg1, arg2, arg3) do {            \
            EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK3(static_name, arg1, arg2, arg3)    \
            IF_EVENT_VALID(event)                                                   \
                EVENT_ITERATE_LISTENERS_BEGIN(event)                                \
                    EVENT_FIRE_IMPL3(event, arg1, arg2, arg3)                       \
                EVENT_ITERATE_LISTENERS_END                                         \
            ELSE_REPORT_EVENT_FIRE_FAILURE(event) } while(0)
#define EVENT_FIRE_FROM_TEMP4(static_name, event, arg1, arg2, arg3, arg4) do {          \
            EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK4(static_name, arg1, arg2, arg3, arg4)  \
            IF_EVENT_VALID(event)                                                       \
                EVENT_ITERATE_LISTENERS_BEGIN(event)                                    \
                    EVENT_FIRE_IMPL4(event, arg1, arg2, arg3, arg4)                     \
                EVENT_ITERATE_LISTENERS_END                                             \
            ELSE_REPORT_EVENT_FIRE_FAILURE(event) } while(0)

#define EVENT_H_NO_EXTERN0(event)                                   EVENT_GEN_DUMMY_FUNCTION_DECL0(event)
#define EVENT_H_NO_EXTERN1(event, arg1_t)                           EVENT_GEN_DUMMY_FUNCTION_DECL1(event, arg1_t)
#define EVENT_H_NO_EXTERN2(event, arg1_t, arg2_t)                   EVENT_GEN_DUMMY_FUNCTION_DECL2(event, arg1_t, arg2_t)
#define EVENT_H_NO_EXTERN3(event, arg1_t, arg2_t, arg3_t)           EVENT_GEN_DUMMY_FUNCTION_DECL3(event, arg1_t, arg2_t, arg3_t)
#define EVENT_H_NO_EXTERN4(event, arg1_t, arg2_t, arg3_t, arg4_t)   EVENT_GEN_DUMMY_FUNCTION_DECL4(event, arg1_t, arg2_t, arg3_t, arg4_t)

/*!
 * @brief Declare an event in a header file with up to 4 arguments.
 * @param event The event name. Should match an event name defined using
 * EVENT_C() in a source file.
 * @param args... The arguments that should be passed to listeners when this
 * event is fired.
@code
EVENT_H(evt_foo, args...);
@endcode
 */
#define EVENT_H0(event)                                         \
    extern struct event_t* event;                               \
    EVENT_H_NO_EXTERN0(event)
#define EVENT_H1(event, arg1)                                   \
    extern struct event_t* event;                               \
    EVENT_H_NO_EXTERN1(event, arg1)
#define EVENT_H2(event, arg1, arg2)                             \
    extern struct event_t* event;                               \
    EVENT_H_NO_EXTERN2(event, arg1, arg2)
#define EVENT_H3(event, arg1, arg2, arg3)                       \
    extern struct event_t* event;                               \
    EVENT_H_NO_EXTERN3(event, arg1, arg2, arg3)
#define EVENT_H4(event, arg1, arg2, arg3, arg4)                 \
    extern struct event_t* event;                               \
    EVENT_H_NO_EXTERN4(event, arg1, arg2, arg3, arg4)
/*!
 * @brief Define an event in a source file with up to 4 arguments.
 * @param event The event name. Should match an event name declared using
 * EVENT_H() in a header file.
 * @param args... The arguments that should be passed to listeners when this
 * event is fired.
@code
EVENT_C(evt_foo, args...);
@endcode
 */
#define EVENT_C0(event)                                                     \
    struct event_t* event = (struct event_t*)0;                             \
    EVENT_GEN_DUMMY_FUNCTION_DEF0(event)
#define EVENT_C1(event, arg1_t)                                             \
    struct event_t* event = (struct event_t*)0;                             \
    EVENT_GEN_DUMMY_FUNCTION_DEF1(event, arg1_t)
#define EVENT_C2(event, arg1_t, arg2_t)                                     \
    struct event_t* event = (struct event_t*)0;                             \
    EVENT_GEN_DUMMY_FUNCTION_DEF2(event, arg1_t, arg2_t)
#define EVENT_C3(event, arg1_t, arg2_t, arg3_t)                             \
    struct event_t* event = (struct event_t*)0;                             \
    EVENT_GEN_DUMMY_FUNCTION_DEF3(event, arg1_t, arg2_t, arg3_t)
#define EVENT_C4(event, arg1_t, arg2_t, arg3_t, arg4_t)                     \
    struct event_t* event = (struct event_t*)0;                             \
    EVENT_GEN_DUMMY_FUNCTION_DEF4(event, arg1_t, arg2_t, arg3_t, arg4_t)

struct service_t
{
    struct plugin_t* plugin;    /* reference to the plugin that owns this service */
    char* directory;
    service_func exec;
    struct type_info_t* type_info;
};

struct event_t
{
    struct plugin_t* plugin;    /* reference to the plugin object that owns this event */
    char* directory;
    struct type_info_t* type_info;
    struct unordered_vector_t listeners; /* holds event_listener_t objects */
};

struct event_listener_t
{
    event_func exec;
    /*
     * See issue #14.
     * C++ does not recognize generic functions. To get around this, store
     * function pointer as void* and use templates to deduce the correct
     * function signature and convert the void* to said function signature
     * before calling.
     */
#ifdef __cplusplus
#   if __cplusplus <= 199711L /* no variadic template support (c++11 not available) */
    void cppexec()
    {
        typedef void (*func_sig)();
        reinterpret_cast<func_sig>(exec)();
    }
    template <class arg1_t>
    void cppexec(const arg1_t& arg1)
    {
        typedef void (*func_sig)(const arg1_t&);
        reinterpret_cast<func_sig>(exec)(arg1);
    }
    template <class arg1_t, class arg2_t>
    void cppexec(const arg1_t& arg1, const arg2_t& arg2)
    {
        typedef void (*func_sig)(const arg1_t&, const arg2_t&);
        reinterpret_cast<func_sig>(exec)(arg1, arg2);
    }
    template <class arg1_t, class arg2_t, class arg3_t>
    void cppexec(const arg1_t& arg1, const arg2_t& arg2, const arg3_t& arg3)
    {
        typedef void (*func_sig)(const arg1_t&, const arg2_t&, const arg3_t&);
        reinterpret_cast<func_sig>(exec)(arg1, arg2, arg3);
    }
    template <class arg1_t, class arg2_t, class arg3_t, class arg4_t>
    void cppexec(const arg1_t& arg1, const arg2_t& arg2, const arg3_t& arg3, const arg4_t& arg4)
    {
        typedef void (*func_sig)(const arg1_t&, const arg2_t&, const arg3_t&, const arg4_t&);
        reinterpret_cast<func_sig>(exec)(arg1, arg2, arg3, arg4);
    }
#   else /* c++11! */
    template <class... args_t>
    void cppexec(args_t&&... args)
    {
        typedef void (*func_sig)(args_t...);
        reinterpret_cast<func_sig>(exec)(args...);
    }
#   endif
#endif
};

#endif /* FRAMEWORK_SERVICE_EVENT_API_H */
