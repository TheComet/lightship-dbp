#include "yaml/config.h"   /* configurations for this plugin */
#include "util/config.h"     /* platform macros and definitions */
#include "util/plugin.h"     /* plugin api */
#include "plugin_yaml/services.h" /* plugin services */
#include "plugin_yaml/events.h"   /* plugin events */
#include "plugin_yaml/parser.h"

struct plugin_t* g_plugin = NULL;

void
create_and_init_plugin(void)
{
    /* create plugin object - host requires this */
    g_plugin = plugin_create();
    
    /* set plugin information - Change this in the file "CMakeLists.txt" */
    plugin_set_info(g_plugin,
            PLUGIN_NAME,         /* name */
            PLUGIN_CATEGORY,     /* category */
            PLUGIN_AUTHOR,       /* author */
            PLUGIN_DESCRIPTION,  /* description */
            PLUGIN_WEBSITE       /* website */
    );
    plugin_set_programming_language(g_plugin,
            PLUGIN_PROGRAMMING_LANGUAGE_C
    );
    plugin_set_version(g_plugin,
            PLUGIN_VERSION_MAJOR,
            PLUGIN_VERSION_MINOR,
            PLUGIN_VERSION_PATCH
    );
}

PLUGIN_INIT()
{
    create_and_init_plugin();
    register_services(g_plugin);
    register_events(g_plugin);
    parser_init();
    return g_plugin;
}

PLUGIN_START()
{
    register_event_listeners(g_plugin);

    return PLUGIN_SUCCESS;
}

PLUGIN_STOP()
{
    parser_deinit();
}

PLUGIN_DEINIT()
{
    plugin_destroy(g_plugin);
}
