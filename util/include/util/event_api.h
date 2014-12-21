#ifndef LIGHTSHIP_UTIL_EVENT_API_HPP
#define LIGHTSHIP_UTIL_EVENT_API_HPP

struct list_t;
struct event_t;

/* event callback function signature */
typedef void (*event_func)(struct event_t*, void*);

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
