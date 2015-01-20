#include "plugin_menu/config.h"   /* configurations for this plugin */
#include "util/config.h"     /* platform macros and definitions */
#include "util/plugin.h"     /* plugin api */
#include "plugin_menu/services.h" /* plugin services */
#include "plugin_menu/events.h"   /* plugin events */
#include "plugin_menu/button.h"

static struct plugin_t* g_plugin = NULL;

void
create_and_init_plugin(void)
{
    /* create plugin object - host requires this */
    g_plugin = plugin_create();
    
    /* set plugin information - Change this in the file "CMakeLists.txt" */
    plugin_set_info(g_plugin,
            MENU_NAME,         /* name */
            MENU_CATEGORY,     /* category */
            MENU_AUTHOR,       /* author */
            MENU_DESCRIPTION,  /* description */
            MENU_WEBSITE       /* website */
    );
    plugin_set_programming_language(g_plugin,
            PLUGIN_PROGRAMMING_LANGUAGE_C
    );
    plugin_set_version(g_plugin,
            MENU_VERSION_MAJOR,
            MENU_VERSION_MINOR,
            MENU_VERSION_PATCH
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
    if(!get_required_services())
        return PLUGIN_FAILURE;
    get_optional_services();
    register_event_listeners(g_plugin);

    button_init();

    return PLUGIN_SUCCESS;
}

PLUGIN_STOP()
{
    /* de-init */
    button_deinit();
}

PLUGIN_DEINIT()
{
    plugin_destroy(g_plugin);
}

