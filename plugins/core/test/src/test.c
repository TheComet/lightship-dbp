#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <util/plugin.h>
#include <test/config.h>
#include <util/config.h>

struct plugin_t* g_plugin;

LIGHTSHIP_PUBLIC_API struct plugin_t* plugin_start(void)
{
    g_plugin = plugin_create();
    
    plugin_set_info(g_plugin,
            "test",                         /* name */
            "system",                       /* category */
            "TheComet",                     /* author */
            "A test plugin for lightship",  /* description */
            "http://github.com/TheComet93/" /* website */
    );
    plugin_set_programming_language(g_plugin,
            PLUGIN_PROGRAMMING_LANGUAGE_C
    );
    plugin_set_version(g_plugin,
            TEST_VERSION_MAJOR,
            TEST_VERSION_MINOR,
            TEST_VERSION_PATCH
    );
    
    printf("hello world! I am a plugin.\n");

	return g_plugin;
}

LIGHTSHIP_PUBLIC_API void plugin_stop(void)
{
	plugin_destroy(g_plugin);
}
