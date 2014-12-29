#include <string.h>
#include "lightship/api.h"
#include "lightship/plugin_manager.h"

struct lightship_api_t g_api;

void
api_init(void)
{
    memset(&g_api, 0, sizeof(struct lightship_api_t));
    
    g_api.plugin_load = plugin_load;
    g_api.plugin_unload = plugin_unload;
    g_api.plugin_get_by_name = plugin_get_by_name;
}
