#ifndef LIGHTSHIP_UTIL_EVENT_API_HPP
#define LIGHTSHIP_UTIL_EVENT_API_HPP

struct list_t;
struct event_t;

/* event callback function signature */
typedef void (*event_func)(struct event_t*, void*);

#define EVENT_LISTENER(name) \
    void name(struct event_t* evt, void* arg)

#define EVENT_FIRE(event, arg) \
    if((event)) \
        ((event)->exec(event, arg)); \
    else \
        fprintf(stderr, "Error: Cannot fire event for it is NULL\n");

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
    char* name;
    event_func exec;
};

#endif /* LIGHTSHIP_UTIL_EVENT_API_HPP */
