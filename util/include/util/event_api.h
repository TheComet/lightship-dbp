#ifndef LIGHTSHIP_UTIL_EVENT_API_HPP
#define LIGHTSHIP_UTIL_EVENT_API_HPP

struct list_t;
struct event_t;

/* event callback function signature */
typedef void (*event_func)(struct event_t*, void*);

#define EVENT_LISTENER(name) \
    void name(struct event_t* evt, void* arg)

#define EVENT_FIRE(event, arg) \
    ((event)->exec(event, arg)

#define EVENT(event) \
    extern struct event_t* event

#define EVENT_IMPL(event) \
    struct event_t* event = NULL

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
