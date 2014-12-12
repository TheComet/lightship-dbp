#include <string.h>
#include <stdlib.h>
#include <lightship/plugin.h>

plugin_t* plugin_create()
{
    plugin_t* plugin = malloc(sizeof(plugin_t));
    plugin_init_plugin(plugin);
    return plugin;
}

void plugin_init_plugin(plugin_t* plugin)
{
    memset(plugin, 0, sizeof(plugin_t));
}

void plugin_destroy(plugin_t* plugin)
{
    free(plugin);
}