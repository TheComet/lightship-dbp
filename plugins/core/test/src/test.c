#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <lightship/plugin.h>

plugin_t plugin;

plugin_t* plugin_start(void)
{
    memset(&plugin, 0, sizeof(plugin_t));
    plugin.info.name = "test";
    plugin.info.version.major = 0;
    plugin.info.version.minor = 0;
    plugin.info.version.patch = 1;
    return &plugin;
}

void plugin_stop(void)
{
}
