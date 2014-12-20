#include "util/plugin.h"

/* host service functions */
typedef struct plugin_t*    (*plugin_load_func)         (struct plugin_info_t*, plugin_search_criteria_t);
typedef void                (*plugin_unload_func)       (struct plugin_t*);
typedef struct plugin_t*    (*plugin_get_by_name_func)  (const char*);
typedef char                (*service_register_func)    (struct plugin_t*, const char*, intptr_t);
typedef char                (*service_unregister_func)  (struct plugin_t*, const char*);
typedef intptr_t            (*service_get_func)         (const char*);

struct lightship_api_t
{
    plugin_load_func plugin_load;
    plugin_unload_func plugin_unload;
    plugin_get_by_name_func plugin_get_by_name;
    service_register_func service_register;
    service_unregister_func service_unregister;
    service_get_func service_get;
};

extern struct lightship_api_t g_api;

void api_init(void);
