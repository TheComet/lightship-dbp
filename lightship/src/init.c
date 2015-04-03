#include "plugin_manager/plugin_manager.h"
#include "plugin_manager/events.h"
#include "plugin_manager/services.h"
#include "plugin_manager/game.h"
#include "util/log.h"
#include "util/memory.h"

#ifdef _DEBUG
static const char* yml_core_plugins = "../../lightship/cfg/core-plugins.yml";
static const char* yml_entry_point = "../../lightship/cfg/entry-point.yml";
#else
static const char* yml_core_plugins = "cfg/core-plugins.yml";
static const char* yml_entry_point = "cfg/entry-point.yml";
#endif

static struct plugin_t* g_plugin_yaml = NULL;
struct game_t* g_local_game = NULL;

typedef void (*start_loop_func)(void);

char
load_core_plugins()
{
    struct plugin_info_t target;
    
    /*!
     * Load the YAML plugin. This is required so the plugin manager can parse
     * the core plugins config file and load the core plugins. If the plugin
     * fails to load, bail.
     */
    target.name = "yaml";
    target.version.major = 0;
    target.version.minor = 0;
    target.version.patch = 1;
    g_plugin_yaml = plugin_load(g_local_game, &target, PLUGIN_VERSION_MINIMUM);
    if(!g_plugin_yaml)
        return 0;
    if(plugin_start(g_local_game, g_plugin_yaml) == PLUGIN_FAILURE)
    {
        llog(LOG_FATAL, NULL, 1, "Failed to start YAML plugin");
        return 0;
    }

    /*
     * Try to load and start the core plugins. If that fails, bail out.
     */
    if(!load_plugins_from_yaml(g_local_game, yml_core_plugins))
    {
        llog(LOG_FATAL, NULL, 1, "Couldn't start all core plugins");
        return 0;
    }
    
    return 1;
}

char
init(void)
{
    /*
     * Initialise memory management first.
     */
    memory_init();
    
    /*
     * Enable logging as soon as possible
     */
    llog_init();
    
    /*
     * Create the local game instance. This is the context that holds all
     * plugins, services, and events together.
     */
    g_local_game = game_create("localhost");
    if(!g_local_game)
        return 0;
    
    /*
     * Services and events should be initialised before anything else, as they
     * register built-in mechanics that are required throughout the rest of the
     * program (such as the log event).
     */
    services_init(g_local_game);
    events_init(g_local_game);
    
    /*
     * Load and start the core plugins specified in the plugins YAML file.
     */
    if(!load_core_plugins())
        return 0;
    
    return 1;
}

void
run_game()
{
    /* 
     * Try to get the main loop service and start running the game
     */
    {
        char* start_service_name;
        uint32_t doc_ID;
        struct service_t* start;
        const char* entry_point_key = "service";

        /* load the yaml file */
        SERVICE_CALL_NAME1(g_local_game, "yaml.load", &doc_ID, PTR(yml_entry_point));
        if(!doc_ID)
        {
            llog(LOG_FATAL, NULL, 1, "Cannot get main loop service");
            return;
        }
        
        /* search for the entry point key and retrieve its value, which is the name of the service to start with */
        SERVICE_CALL_NAME2(g_local_game, "yaml.get_value", &start_service_name, doc_ID, PTR(entry_point_key));
        if(!start_service_name)
        {
            llog(LOG_FATAL, NULL, 3, "Cannot get value of \"service\" in \"", yml_entry_point ,"\"");
            SERVICE_CALL_NAME1(g_local_game, "yaml.destroy", SERVICE_NO_RETURN, doc_ID);
            return;
        }

        /* with the service name retrieved, try to call it */
        start = service_get(g_local_game, start_service_name);
        SERVICE_CALL_NAME1(g_local_game, "yaml.destroy", SERVICE_NO_RETURN, doc_ID);
        if(!start)
        {
            llog(LOG_FATAL, NULL, 1, "Cannot get main loop service");
            return;
        }
        
        SERVICE_CALL0(start, SERVICE_NO_RETURN);
    
    }
}

void
deinit(void)
{
    plugin_manager_deinit(g_local_game);
    llog_deinit();
    events_deinit(g_local_game);
    services_deinit();
    game_destroy(g_local_game);
    
    /*
     * De-init memory management last
     */
    memory_deinit();
}
