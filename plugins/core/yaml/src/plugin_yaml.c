#include "plugin_manager/plugin.h"  /* plugin api */
#include "plugin_yaml/config.h"     /* configurations for this plugin */
#include "plugin_yaml/services.h"   /* plugin services */
#include "plugin_yaml/events.h"     /* plugin events */
#include "plugin_yaml/parser.h"
#include "plugin_yaml/glob.h"

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
PLUGIN_YAML_PUBLIC_API PLUGIN_INIT()
{
    struct plugin_t* plugin;

    /* init global data */
    glob_create(game);
    
    /* init plugin */
    plugin = create_and_init_plugin(game);
    register_services(game, plugin);
    register_events(game, plugin);
    parser_init(game);
    
    return plugin;
}

/* ------------------------------------------------------------------------- */
PLUGIN_YAML_PUBLIC_API PLUGIN_START()
{
    register_event_listeners(game, get_global(game)->plugin);

    return PLUGIN_SUCCESS;
}

/* ------------------------------------------------------------------------- */
PLUGIN_YAML_PUBLIC_API PLUGIN_STOP()
{
    parser_deinit(game);
}

/* ------------------------------------------------------------------------- */
PLUGIN_YAML_PUBLIC_API PLUGIN_DEINIT()
{
    plugin_destroy(get_global(game)->plugin);
    glob_destroy(game);
}
