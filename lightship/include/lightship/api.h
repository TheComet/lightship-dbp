#ifndef LIGHTSHIP_API_HPP
#define LIGHTSHIP_API_HPP

#include "util/plugin.h"
#include "util/event_api.h"

/* host service functions */
struct lightship_api_t
{
    /* plugin manager */
    struct plugin_t*    (*plugin_load)                     (struct plugin_info_t*, plugin_search_criteria_t);
    void                (*plugin_unload)                   (struct plugin_t*);
    struct plugin_t*    (*plugin_get_by_name)              (const char*);
    /* services */
    char                (*service_register)                (struct plugin_t*, const char*, intptr_t);
    char                (*service_unregister)              (struct plugin_t*, const char*);
    intptr_t            (*service_get)                     (const char*);
    /* events */
    struct event_t*     (*event_create)                    (struct plugin_t*, const char*);
    char                (*event_destroy)                   (struct event_t*);
    void                (*event_destroy_plugin_event)      (struct plugin_t*, const char*);
    void                (*event_destroy_all_plugin_events) (struct plugin_t*);
    struct event_t*     (*event_get)                       (const char*);
    char                (*event_register_listener)         (struct plugin_t*, const char*, event_func);
    char                (*event_unregister_listener)       (const char*, const char*);
    void                (*event_unregister_all_listeners)  (struct event_t*);
};

extern struct lightship_api_t g_api;

void api_init(void);

#endif /* LIGHTSHIP_API_HPP */
