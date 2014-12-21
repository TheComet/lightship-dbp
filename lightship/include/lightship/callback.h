#include "util/plugin_api.h"

struct plugin_t;
struct list_t;

struct callback_t
{
    char* name;
    intptr_t exec;
};

/*!
 * @brief Creates a new callback and registers it to the list.
 */
struct callback_t* callback_create_and_register(struct list_t* cb_list,
                                                struct plugin_t* plugin,
                                                const char* name,
                                                intptr_t exec);

char callback_destroy_and_unregister(struct list_t* cb_list,
                                     struct plugin_t* plugin,
                                     const char* name);

void callback_destroy_all(struct list_t* cb_list,
                          struct plugin_t* plugin);

intptr_t callback_get(struct list_t* cb_list, const char* name);
