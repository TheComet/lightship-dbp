#ifndef LIGHTSHIP_UTIL_EVENT_API_H
#define LIGHTSHIP_UTIL_EVENT_API_H

#include <stdio.h>
#include "util/linked_list.h"

struct list_t;
struct event_t;

/* event callback function signature */
typedef void (*event_callback_func)();

/* -------------------------------------------------------------------------
 * The following is the event listener and dispatch system implemented
 * partially in macros. We have no access to variadic macros so it may
 * appear a little bloated.
 * -----------------------------------------------------------------------*/

/* register listeners with up to 4 receiving function parameters */
#define EVENT_LISTENER(name) \
    void name(struct event_t* evt)
#define EVENT_LISTENER1(name, arg) \
    void name(struct event_t* evt, arg)
#define EVENT_LISTENER2(name, arg1, arg2) \
    void name(struct event_t* evt, arg1, arg2)
#define EVENT_LISTENER3(name, arg1, arg2, arg3) \
    void name(struct event_t* evt, arg1, arg2, arg3)
#define EVENT_LISTENER4(name, arg1, arg2, arg3, arg4) \
    void name(struct event_t* evt, arg1, arg2, arg3, arg4)

/* the actual function call to the listener */
#define EVENT_FIRE_IMPL0(event) \
            listener->exec(event, NULL);
#define EVENT_FIRE_IMPL1(event, arg) \
            listener->exec(event, arg);
#define EVENT_FIRE_IMPL2(event, arg1, arg2) \
            listener->exec(event, arg1, arg2);
#define EVENT_FIRE_IMPL3(event, arg1, arg2, arg3) \
            listener->exec(event, arg1, arg2, arg3);
#define EVENT_FIRE_IMPL4(event, arg1, arg2, arg3, arg4) \
            listener->exec(event, arg1, arg2, arg3, arg4);

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
            LIST_FOR_EACH((event)->listeners, struct event_listener_t, listener) \
            {
#define EVENT_ITERATE_LISTENERS_END \
            }

/* fire events with up to 4 function parameters */
#define EVENT_FIRE(event) \
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

#define EVENT_H(event) \
    extern struct event_t* event;

#define EVENT_C(event) \
    struct event_t* event = (void*)0;

struct event_t
{
    char* name;
    struct list_t* listeners; /* holds event_listener_t objects */
};

struct event_listener_t
{
    char* namespace;
    event_callback_func exec;
};

#endif /* LIGHTSHIP_UTIL_EVENT_API_H */
