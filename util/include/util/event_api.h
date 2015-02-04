#ifndef LIGHTSHIP_UTIL_EVENT_API_H
#define LIGHTSHIP_UTIL_EVENT_API_H

#include <stdio.h>
#include "util/config.h"
#include "util/unordered_vector.h"

C_HEADER_BEGIN

struct event_t;

/* event callback function signature */
typedef void (*event_callback_func)();

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
@code
EVENT_LISTENER1(on_event, int arg);

...

EVENT_LISTENER1(on_event, int arg)
{
}
@endcode
 * @param[in] name The name to give the event listener. Usually something like
 * *on_event*.
 * @param[in] arg... The arguments to receive from the event.
 * @note The event listener must have the same signature as the fired event.
 */
#define EVENT_LISTENER0(name) \
    void name(const struct event_t* evt)
#define EVENT_LISTENER1(name, arg) \
    void name(const struct event_t* evt, arg)
#define EVENT_LISTENER2(name, arg1, arg2) \
    void name(const struct event_t* evt, arg1, arg2)
#define EVENT_LISTENER3(name, arg1, arg2, arg3) \
    void name(const struct event_t* evt, arg1, arg2, arg3)
#define EVENT_LISTENER4(name, arg1, arg2, arg3, arg4) \
    void name(const struct event_t* evt, arg1, arg2, arg3, arg4)

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
 * see issue #14
 * When compiling with C++, let a call to cppexec delegate the call to exec.
 * When compiling with C89, call exec directly.
 */
#ifdef __cplusplus
#   define EVENT_EXEC_FUNC cppexec
#else
#   define EVENT_EXEC_FUNC exec
#endif

/* the actual function call to the listener */
#define EVENT_FIRE_IMPL0(event) \
            listener->EVENT_EXEC_FUNC(event);
#define EVENT_FIRE_IMPL1(event, arg) \
            listener->EVENT_EXEC_FUNC(event, arg);
#define EVENT_FIRE_IMPL2(event, arg1, arg2) \
            listener->EVENT_EXEC_FUNC(event, arg1, arg2);
#define EVENT_FIRE_IMPL3(event, arg1, arg2, arg3) \
            listener->EVENT_EXEC_FUNC(event, arg1, arg2, arg3);
#define EVENT_FIRE_IMPL4(event, arg1, arg2, arg3, arg4) \
            listener->EVENT_EXEC_FUNC(event, arg1, arg2, arg3, arg4);

/*
 * In debug mode, we want to print the stack trace if an event object
 * is NULL. In release mode, we can skip the stack trace and skip the check
 * for event being NULL.
 */
#ifdef _DEBUG
#   include <stdlib.h>
#   include "util/backtrace.h"
#   include "util/log.h"

/* this is the check for whether the event object is NULL or not */
#   define IF_EVENT_VALID(event) \
                if((event)) {

/* 
 * This is the closure of the IF_EVENT_VALID condition, which will print
 * the stacktrace.
 */
#   define REPORT_EVENT_FIRE_FAILURE \
                } else { \
                    uint32_t size, i; \
                    char** backtrace = get_backtrace(&size); \
                    llog(LOG_ERROR, NULL, 1, "Cannot fire event for it is NULL"); \
                    llog_indent("Backtrace"); \
                    for(i = 0; i != size; ++i) \
                        llog(LOG_ERROR, NULL, 1, backtrace[i]); \
                    if(backtrace) free(backtrace); \
                    llog_unindent(); \
                }

#else  /* _DEBUG */
#   define REPORT_EVENT_FIRE_FAILURE
#   define IF_EVENT_VALID(event)
#endif /* _DEBUG */

/* used to iterate over the listeners of an event */
#define EVENT_ITERATE_LISTENERS_BEGIN(event) \
            UNORDERED_VECTOR_FOR_EACH(&(event)->listeners, struct event_listener_t, listener) \
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
#define EVENT_FIRE0(event) do { \
            EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK0(event) \
            IF_EVENT_VALID(event) \
                EVENT_ITERATE_LISTENERS_BEGIN(event) \
                    EVENT_FIRE_IMPL0(event) \
                EVENT_ITERATE_LISTENERS_END \
            REPORT_EVENT_FIRE_FAILURE } while(0)
#define EVENT_FIRE1(event, arg) do { \
            EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK1(event, arg) \
            IF_EVENT_VALID(event) \
                EVENT_ITERATE_LISTENERS_BEGIN(event) \
                    EVENT_FIRE_IMPL1(event, arg) \
                EVENT_ITERATE_LISTENERS_END \
            REPORT_EVENT_FIRE_FAILURE } while(0)
#define EVENT_FIRE2(event, arg1, arg2) do { \
            EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK2(event, arg1, arg2) \
            IF_EVENT_VALID(event) \
                EVENT_ITERATE_LISTENERS_BEGIN(event) \
                    EVENT_FIRE_IMPL2(event, arg1, arg2) \
                EVENT_ITERATE_LISTENERS_END \
            REPORT_EVENT_FIRE_FAILURE } while(0)
#define EVENT_FIRE3(event, arg1, arg2, arg3) do { \
            EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK3(event, arg1, arg2, arg3) \
            IF_EVENT_VALID(event) \
                EVENT_ITERATE_LISTENERS_BEGIN(event) \
                    EVENT_FIRE_IMPL3(event, arg1, arg2, arg3) \
                EVENT_ITERATE_LISTENERS_END \
            REPORT_EVENT_FIRE_FAILURE } while(0)
#define EVENT_FIRE4(event, arg1, arg2, arg3, arg4) do { \
            EVENT_DUMMY_FUNCTION_SIGNATURE_CHECK4(event, arg1, arg2, arg3, arg4) \
            IF_EVENT_VALID(event) \
                EVENT_ITERATE_LISTENERS_BEGIN(event) \
                    EVENT_FIRE_IMPL4(event, arg1, arg2, arg3, arg4) \
                EVENT_ITERATE_LISTENERS_END \
            REPORT_EVENT_FIRE_FAILURE } while(0)

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
#define EVENT_H0(event) \
    EVENT_GEN_DUMMY_FUNCTION_DECL0(event) \
    extern struct event_t* event
#define EVENT_H1(event, arg1_t) \
    EVENT_GEN_DUMMY_FUNCTION_DECL1(event, arg1_t) \
    extern struct event_t* event
#define EVENT_H2(event, arg1_t, arg2_t) \
    EVENT_GEN_DUMMY_FUNCTION_DECL2(event, arg1_t, arg2_t) \
    extern struct event_t* event
#define EVENT_H3(event, arg1_t, arg2_t, arg3_t) \
    EVENT_GEN_DUMMY_FUNCTION_DECL3(event, arg1_t, arg2_t, arg3_t) \
    extern struct event_t* event
#define EVENT_H4(event, arg1_t, arg2_t, arg3_t, arg4_t) \
    EVENT_GEN_DUMMY_FUNCTION_DECL4(event, arg1_t, arg2_t, arg3_t, arg4_t) \
    extern struct event_t* event

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
#define EVENT_C0(event) \
    EVENT_GEN_DUMMY_FUNCTION_DEF0(event) \
    struct event_t* event = (struct event_t*)0
#define EVENT_C1(event, arg1_t) \
    EVENT_GEN_DUMMY_FUNCTION_DEF1(event, arg1_t) \
    struct event_t* event = (struct event_t*)0
#define EVENT_C2(event, arg1_t, arg2_t) \
    EVENT_GEN_DUMMY_FUNCTION_DEF2(event, arg1_t, arg2_t) \
    struct event_t* event = (struct event_t*)0
#define EVENT_C3(event, arg1_t, arg2_t, arg3_t) \
    EVENT_GEN_DUMMY_FUNCTION_DEF3(event, arg1_t, arg2_t, arg3_t) \
    struct event_t* event = (struct event_t*)0
#define EVENT_C4(event, arg1_t, arg2_t, arg3_t, arg4_t) \
    EVENT_GEN_DUMMY_FUNCTION_DEF4(event, arg1_t, arg2_t, arg3_t, arg4_t) \
    struct event_t* event = (struct event_t*)0
    
C_HEADER_END

struct event_t
{
    char* name;
    struct unordered_vector_t listeners; /* holds event_listener_t objects */
};

struct event_listener_t
{
    char* name_space;
    event_callback_func exec;
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

#endif /* LIGHTSHIP_UTIL_EVENT_API_H */
