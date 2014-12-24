#ifndef LIGHTSHIP_API_H
#define LIGHTSHIP_API_H

#include "util/plugin.h"

/* host service functions */
struct lightship_api_t
{
    /* plugin manager */
    struct plugin_t*    (*plugin_load)                     (struct plugin_info_t*, plugin_search_criteria_t);
    void                (*plugin_unload)                   (struct plugin_t*);
    struct plugin_t*    (*plugin_get_by_name)              (const char*);
};

extern struct lightship_api_t g_api;

void api_init(void);

#endif /* LIGHTSHIP_API_H */
