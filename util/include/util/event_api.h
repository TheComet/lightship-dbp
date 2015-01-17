#ifndef LIGHTSHIP_UTIL_EVENT_API_H
#define LIGHTSHIP_UTIL_EVENT_API_H

#include <stdio.h>
#include "util/config.h"
#include "util/unordered_vector.h"

#ifdef __cplusplus
#include <functional>
#endif

C_HEADER_BEGIN

struct list_t;
struct event_t;

/* event callback function signature */
typedef void (*event_callback_func)();

/* ------------------------------------------------------------------------- *
 * The following is the event listener and dispatch system implemented       *
 * partially in macros. We have no access to variadic macros so it may       *
 * appear a little bloated.                                                  *
 * ------------------------------------------------------------------------- */

/*!
 * @brief Register listeners with up to 4 receiving function parameters
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

/* see issue #14 */
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
                    intptr_t size, i; \
                    char** backtrace = get_backtrace(&size); \
                    llog(LOG_ERROR, 1, "Cannot fire event for it is NULL"); \
                    llog_indent("Backtrace"); \
                    for(i = 0; i != size; ++i) \
                        llog(LOG_ERROR, 1, backtrace[i]); \
                    free(backtrace); \
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

/*!
 * @brief Fires the specified event with the specified arguments.
 * @param[in] event The event to fire. Should be declared with EVENT_C/EVENT_H
 * and be registered with the host program with event_register().
 * @param[in] args... The arguments to fire along with the event.
 */
#define EVENT_FIRE0(event) \
            IF_EVENT_VALID(event) \
                EVENT_ITERATE_LISTENERS_BEGIN(event) \
                    EVENT_FIRE_IMPL0(event) \
                EVENT_ITERATE_LISTENERS_END \
            REPORT_EVENT_FIRE_FAILURE
#define EVENT_FIRE1(event, arg) \
            IF_EVENT_VALID(event) \
                EVENT_ITERATE_LISTENERS_BEGIN(event) \
                    EVENT_FIRE_IMPL1(event, arg) \
                EVENT_ITERATE_LISTENERS_END \
            REPORT_EVENT_FIRE_FAILURE
#define EVENT_FIRE2(event, arg1, arg2) \
            IF_EVENT_VALID(event) \
                EVENT_ITERATE_LISTENERS_BEGIN(event) \
                    EVENT_FIRE_IMPL2(event, arg1, arg2) \
                EVENT_ITERATE_LISTENERS_END \
            REPORT_EVENT_FIRE_FAILURE
#define EVENT_FIRE3(event, arg1, arg2, arg3) \
            IF_EVENT_VALID(event) \
                EVENT_ITERATE_LISTENERS_BEGIN(event) \
                    EVENT_FIRE_IMPL3(event, arg1, arg2, arg3) \
                EVENT_ITERATE_LISTENERS_END \
            REPORT_EVENT_FIRE_FAILURE
#define EVENT_FIRE4(event, arg1, arg2, arg3, arg4) \
            IF_EVENT_VALID(event) \
                EVENT_ITERATE_LISTENERS_BEGIN(event) \
                    EVENT_FIRE_IMPL4(event, arg1, arg2, arg3, arg4) \
                EVENT_ITERATE_LISTENERS_END \
            REPORT_EVENT_FIRE_FAILURE

/*!
 * @brief Declare an event in a header file.
 * @param even The event name. Should match an event name defined using
 * EVENT_C() in a source file.
@code
EVENT_H(evt_foo);
@endcode
 */
#define EVENT_H(event) \
    extern struct event_t* event;

/*!
 * @brief Define an event in a source file.
 * @param event The event name. Should match an event name declared using
 * EVENT_H() in a header file.
@code
EVENT_C(evt_foo);
@endcode
 */
#define EVENT_C(event) \
    struct event_t* event = (void*)0;
    
C_HEADER_END

struct event_t
{
    char* name;
    struct unordered_vector_t listeners; /* holds event_listener_t objects */
};

struct event_listener_t
{
    char* name_space;
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
    event_callback_func exec;
};

#endif /* LIGHTSHIP_UTIL_EVENT_API_H */
