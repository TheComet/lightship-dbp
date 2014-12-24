#ifndef LIGHTSHIP_UTIL_EVENT_API_H
#define LIGHTSHIP_UTIL_EVENT_API_H

#include <stdio.h>

struct list_t;
struct event_t;

/* event callback function signature */
typedef void (*event_func)(struct event_t*, void*);

#define EVENT_LISTENER(name, arg) \
    void name(struct event_t* evt, arg)

#ifdef _DEBUG
#   include <stdlib.h>
#   include "util/backtrace.h"
#   include "util/log.h"

#   define EVENT_FIRE(event, arg) \
        if((event)) \
            ((event)->exec(event, (void*)arg)); \
        else \
        { \
            intptr_t size, i; \
            char** backtrace = get_backtrace(&size); \
            llog(LOG_ERROR, 1, "Cannot fire event for it is NULL\n"); \
            for(i = 0; i != size; ++i) \
                llog(LOG_ERROR, 1, backtrace[i]); \
            free(backtrace); \
        }
#else
#   define EVENT_FIRE(event, arg) \
        ((event)->exec(event, (void*)arg));
#endif

#define EVENT_H(event) \
    extern struct event_t* event;

#define EVENT_C(event) \
    struct event_t* event = (void*)0;

struct event_t
{
    char* name;
    event_func exec;
    struct list_t* listeners; /* holds event_listener_t objects */
};

struct event_listener_t
{
    char* namespace;
    event_func exec;
};

#endif /* LIGHTSHIP_UTIL_EVENT_API_H */
