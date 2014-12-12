#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <lightship/plugin.h>
#include <test/config.h>

plugin_t* g_plugin;

void plugin_start(plugin_t* plugin)
{
    g_plugin = plugin;
    
    plugin_set_info(g_plugin,
            "test",
            "TheComet",
            "A test plugin for lightship",
            "http://github.com/TheComet93/"
    );
    plugin_set_programming_language(g_plugin,
            PLUGIN_PROGRAMMING_LANGUAGE_C
    );
    plugin_set_version(g_plugin,
            TEST_VERSION_MAJOR,
            TEST_VERSION_MINOR,
            TEST_VERSION_PATCH
    );
}

void plugin_stop(void)
{
}
