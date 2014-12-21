#include "lightship/api.h" /* lightship API so we can register and call services */
#include "util/config.h"   /* platform macros and definitions */
#include "util/plugin.h"   /* plugin api */
#include "main_loop/config.h"  /* configurations for this plugin */
#include "main_loop/main_loop.h"
#include "util/event_api.h"
#include <stdio.h>

struct plugin_t* g_plugin = NULL;

LIGHTSHIP_PUBLIC_API struct plugin_t* plugin_init(struct lightship_api_t* api)
{
    /* create plugin object - host requires this */
    g_plugin = plugin_create();
    
    /* set plugin information */
    plugin_set_info(g_plugin,
            "main_loop",   /* name */
            "system",   /* category */
            "TheComet",   /* author */
            "Controls the speed of game loop and render loop",   /* description */
            "http://github.com/TheComet93/"    /* website */
    );
    plugin_set_programming_language(g_plugin,
            PLUGIN_PROGRAMMING_LANGUAGE_C
    );
    plugin_set_version(g_plugin,
            MAIN_LOOP_VERSION_MAJOR,
            MAIN_LOOP_VERSION_MINOR,
            MAIN_LOOP_VERSION_PATCH
    );

    api->service_register(g_plugin, "start", (intptr_t)main_loop_start);
    api->service_register(g_plugin, "stop", (intptr_t)main_loop_stop);
    
    register_events(g_plugin, api);

    return g_plugin;
}

LIGHTSHIP_PUBLIC_API plugin_result_t plugin_start(void)
{
    register_listeners(g_plugin, &g_api);
    return PLUGIN_SUCCESS;
}

LIGHTSHIP_PUBLIC_API void plugin_stop(void)
{
    plugin_destroy(g_plugin);
}
