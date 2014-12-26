#include "input/config.h"   /* configurations for this plugin */
#include "lightship/api.h"   /* lightship API so we can register and call services */
#include "util/config.h"     /* platform macros and definitions */
#include "util/plugin.h"     /* plugin api */
#include "plugin_input/services.h" /* plugin services */
#include "plugin_input//events.h"   /* plugin events */

struct plugin_t* g_plugin = NULL;

void create_and_init_plugin(void)
{
    /* create plugin object - host requires this */
    g_plugin = plugin_create();
    
    /* set plugin information - Change this in the file "CMakeLists.txt" */
    plugin_set_info(g_plugin,
            INPUT_NAME,         /* name */
            INPUT_CATEGORY,     /* category */
            INPUT_AUTHOR,       /* author */
            INPUT_DESCRIPTION,  /* description */
            INPUT_WEBSITE       /* website */
    );
    plugin_set_programming_language(g_plugin,
            PLUGIN_PROGRAMMING_LANGUAGE_C
    );
    plugin_set_version(g_plugin,
            INPUT_VERSION_MAJOR,
            INPUT_VERSION_MINOR,
            INPUT_VERSION_PATCH
    );
}

PLUGIN_INIT()
{
    create_and_init_plugin();
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
