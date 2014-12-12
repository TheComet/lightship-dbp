#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <lightship/plugin.h>
#include <test/config.h>

plugin_t* g_plugin;

void plugin_start(plugin_t* plugin)
{
    g_plugin = plugin;
    
    /* set information about this plugin */
    g_plugin->info.name = "test";
    g_plugin->info.author = "TheComet";
    g_plugin->info.description = "A test plugin for lightship";
    g_plugin->info.website = "github.com/TheComet93/";
    
    g_plugin->info.language = PLUGIN_PROGRAMMING_LANGUAGE_C;
    
    g_plugin->info.version.major = TEST_VERSION_MAJOR;
    g_plugin->info.version.minor = TEST_VERSION_MINOR;
    g_plugin->info.version.patch = TEST_VERSION_PATCH;
}

void plugin_stop(void)
{
}
