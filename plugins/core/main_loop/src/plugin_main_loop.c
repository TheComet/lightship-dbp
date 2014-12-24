#include "lightship/api.h" /* lightship API so we can register and call services */
#include "util/config.h"   /* platform macros and definitions */
#include "util/plugin.h"   /* plugin api */
#include "main_loop/config.h"  /* configurations for this plugin */
#include "main_loop/services.h"
#include "main_loop/events.h"

#include "main_loop/main_loop.h"
#include <stdio.h>

struct plugin_t* g_plugin = NULL;

PLUGIN_INIT()
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

    register_services(g_plugin);
    register_events(g_plugin);

    return g_plugin;
}

PLUGIN_START()
{
    register_event_listeners(g_plugin);
    return PLUGIN_SUCCESS;
}

PLUGIN_STOP()
{
    plugin_destroy(g_plugin);
}
