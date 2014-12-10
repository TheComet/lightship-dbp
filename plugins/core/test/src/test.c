#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <lightship/plugin.h>

plugin_t plugin;

plugin_t* plugin_start(void)
{
    memset(&plugin, 0, sizeof(plugin_t));
    plugin.info.name = "test";
    return &plugin;
}

void plugin_stop(void)
{
}
