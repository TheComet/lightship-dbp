#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <lightship/plugin.h>
#include <test/config.h>

plugin_t plugin;

plugin_t* plugin_start(void)
{
    memset(&plugin, 0, sizeof(plugin_t));
    plugin.info.name = "test";
    plugin.info.version.major = TEST_VERSION_MAJOR;
    plugin.info.version.minor = TEST_VERSION_MINOR;
    plugin.info.version.patch = TEST_VERSION_PATCH;
    return &plugin;
}

void plugin_stop(void)
{
}
