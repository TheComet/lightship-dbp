#include "plugin_menu/config.h"   /* configurations for this plugin */
#include "util/config.h"     /* platform macros and definitions */
#include "util/plugin.h"     /* plugin api */
#include "plugin_menu/services.h" /* plugin services */
#include "plugin_menu/events.h"   /* plugin events */
#include "plugin_menu/button.h"
#include "plugin_menu/menu.h"

#include "util/services.h"

static struct plugin_t* g_plugin = NULL;
static char* g_menu;

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
    return g_plugin;
}

PLUGIN_START()
{
    if(!get_required_services())
        return PLUGIN_FAILURE;
    get_optional_services();
    register_event_listeners(g_plugin);

    button_init();
    menu_init();
    
    {
        struct service_t* menu_load_service = service_get("menu.load");
#ifdef _DEBUG
        const char* menu_file_name = "../../plugins/core/menu/cfg/menu.yml";
#else
        const char* menu_file_name = "cfg/menu.yml";
#endif
        SERVICE_CALL1(menu_load_service, &g_menu, *menu_file_name);
    }

    return PLUGIN_SUCCESS;
}

PLUGIN_STOP()
{
    /* de-init */
    struct service_t* menu_destroy_service = service_get("menu.destroy");
    SERVICE_CALL1(menu_destroy_service, SERVICE_NO_RETURN, *g_menu);
    
    menu_deinit();
    button_deinit();
}

PLUGIN_DEINIT()
{
    plugin_destroy(g_plugin);
}

