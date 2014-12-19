#include "util/pstdint.h"

struct plugin_t;
struct service_info_t;
struct service_arguments_;

typedef void (*service_func)(uint32_t nparams,
                             const void* param,
                             uint32_t nret_vals,
                             void** ret_vals);

typedef int (*service_register_func)(struct plugin_t*,
                                     const char*,
                                     service_func);

struct service_t
{
    char* name;
    service_func exec;
};

void services_init(void);

char service_register(struct plugin_t* plugin,
                     const char* name,
                     service_func exec);
                     

char service_unregister(struct plugin_t* plugin,
                       const char* name);

void service_unregister_all(struct plugin_t* plugin);

service_func service_get(const char* name);
