#include "util/plugin_api.h"

struct list_t;

struct callback_t
{
    char* name;
    intptr_t exec;
};

char callback_register(struct list_t* cb_list,
                       struct plugin_t* plugin,
                       const char* name,
                       intptr_t exec);

char callback_unregister(struct list_t* cb_list,
                         struct plugin_t* plugin,
                         const char* name);

void callback_unregister_all(struct list_t* cb_list,
                             struct plugin_t* plugin);

intptr_t callback_get(struct list_t* cb_list, const char* name);
