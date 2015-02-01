#include "util/config.h"   /* platform macros and definitions */
#include "util/plugin.h"   /* plugin api */
#include "plugin_main_loop/config.h"  /* configurations for this plugin */
#include "plugin_main_loop/services.h"
#include "plugin_main_loop/events.h"

#include "plugin_main_loop/main_loop.h"
#include <stdio.h>

struct plugin_t* g_plugin = NULL;

PLUGIN_INIT()
{
    /* create plugin object - host requires this */
    g_plugin = plugin_create();
    
    /* set plugin information */
    plugin_set_info(g_plugin,
            PLUGIN_NAME,            /* name */
            PLUGIN_CATEGORY,        /* category */
            PLUGIN_AUTHOR,          /* author */
            PLUGIN_DESCRIPTION,     /* description */
            PLUGIN_WEBSITE          /* website */
    );
    plugin_set_programming_language(g_plugin,
            PLUGIN_PROGRAMMING_LANGUAGE_C
    );
    plugin_set_version(g_plugin,
            PLUGIN_VERSION_MAJOR,
            PLUGIN_VERSION_MINOR,
            PLUGIN_VERSION_PATCH
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
}

PLUGIN_DEINIT()
{
    plugin_destroy(g_plugin);
}
