#include "plugin_menu/config.h"   /* configurations for this plugin */
#include "plugin_manager/plugin.h"     /* plugin api */
#include "plugin_menu/services.h" /* plugin services */
#include "plugin_menu/events.h"   /* plugin events */
#include "plugin_menu/glob.h"
#include "plugin_menu/button.h"
#include "plugin_menu/menu.h"

#include "plugin_manager/services.h"

/* ------------------------------------------------------------------------- */
PLUGIN_MENU_PUBLIC_API PLUGIN_INIT()
{
    struct plugin_t* plugin;
    
    /* init global data */
    glob_create(game);
    
    /* init plugin */
    plugin = plugin_create(game);
    get_global(game)->plugin = plugin;
    
    /* set plugin information - Change this in the file "CMakeLists.txt" */
    plugin_set_info(plugin,
            PLUGIN_NAME,         /* name */
            PLUGIN_CATEGORY,     /* category */
            PLUGIN_AUTHOR,       /* author */
            PLUGIN_DESCRIPTION,  /* description */
            PLUGIN_WEBSITE       /* website */
    );
    plugin_set_programming_language(plugin,
            PLUGIN_PROGRAMMING_LANGUAGE_C
    );
    plugin_set_version(plugin,
            PLUGIN_VERSION_MAJOR,
            PLUGIN_VERSION_MINOR,
            PLUGIN_VERSION_PATCH
    );

    register_services(plugin);
    register_events(plugin);
    
    return plugin;
}

/* ------------------------------------------------------------------------- */
PLUGIN_MENU_PUBLIC_API PLUGIN_START()
{
    struct glob_t* g = get_global(game);

    if(!get_required_services(get_global(game)->plugin))
        return PLUGIN_FAILURE;
    get_optional_services(get_global(game)->plugin);
    register_event_listeners(get_global(game)->plugin);

    element_init(g);
    button_init(g);
    menu_init(g);

    return PLUGIN_SUCCESS;
}

/* ------------------------------------------------------------------------- */
PLUGIN_MENU_PUBLIC_API PLUGIN_STOP()
{
    struct service_t* menu_destroy_service;
    struct glob_t* g;

    /* de-init */
    g = get_global(game);
    menu_deinit(g);
    button_deinit(g);
}

/* ------------------------------------------------------------------------- */
PLUGIN_MENU_PUBLIC_API PLUGIN_DEINIT()
{
    plugin_destroy(get_global(game)->plugin);
    glob_destroy(game);
}
