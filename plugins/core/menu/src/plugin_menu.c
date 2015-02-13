#include "plugin_menu/config.h"   /* configurations for this plugin */
#include "plugin_manager/plugin.h"     /* plugin api */
#include "plugin_menu/services.h" /* plugin services */
#include "plugin_menu/events.h"   /* plugin events */
#include "plugin_menu/glob.h"
#include "plugin_menu/button.h"
#include "plugin_menu/menu.h"

#include "plugin_manager/services.h"

static char* g_menu;

/* ------------------------------------------------------------------------- */
struct plugin_t*
create_and_init_plugin(struct game_t* game)
{
    /* create plugin object - host requires this */
    struct plugin_t* plugin = plugin_create(game);
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
    
    return plugin;
}

/* ------------------------------------------------------------------------- */
PLUGIN_MENU_PUBLIC_API PLUGIN_INIT()
{
    struct plugin_t* plugin;
    
    /* init global data */
    glob_create(game);
    
    /* init plugin */
    plugin = create_and_init_plugin(game);
    register_services(plugin);
    register_events(plugin);
    return plugin;
}

/* ------------------------------------------------------------------------- */
PLUGIN_MENU_PUBLIC_API PLUGIN_START()
{
    if(!get_required_services(get_global(game)->plugin))
        return PLUGIN_FAILURE;
    get_optional_services(get_global(game)->plugin);
    register_event_listeners(get_global(game)->plugin);

    button_init();
    menu_init();
    
    {
        struct service_t* menu_load_service = service_get(game, "menu.load");
#ifdef _DEBUG
        const char* menu_file_name = "../../plugins/core/menu/cfg/menu.yml";
#else
        const char* menu_file_name = "cfg/menu.yml";
#endif
        SERVICE_CALL1(menu_load_service, &g_menu, PTR(menu_file_name));
    }

    return PLUGIN_SUCCESS;
}

/* ------------------------------------------------------------------------- */
PLUGIN_MENU_PUBLIC_API PLUGIN_STOP()
{
    /* de-init */
    struct service_t* menu_destroy_service = service_get(game, "menu.destroy");
    SERVICE_CALL1(menu_destroy_service, SERVICE_NO_RETURN, PTR(g_menu));
    
    menu_deinit();
    button_deinit();
}

/* ------------------------------------------------------------------------- */
PLUGIN_MENU_PUBLIC_API PLUGIN_DEINIT()
{
    plugin_destroy(get_global(game)->plugin);
    glob_destroy(game);
}
