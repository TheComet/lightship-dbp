#include "plugin_manager/plugin_manager.h"
#include "plugin_manager/events.h"
#include "plugin_manager/services.h"
#include "plugin_manager/game.h"
#include "util/log.h"
#include "util/memory.h"

struct ptree_t;

#ifdef _DEBUG
static const char* yml_settings = "../../lightship/cfg/settings.yml";
#else
static const char* yml_settings = "cfg/settings.yml";
#endif

static struct plugin_t* g_plugin_yaml = NULL;
static uint32_t g_settings_doc_id;
struct game_t* g_localhost = NULL;
struct game_t* g_client = NULL;

typedef void (*start_loop_func)(void);

/* -------------------------------------------------------------------------- */
char
load_core_plugins(struct game_t* game)
{
    
    struct plugin_info_t target;
    struct ptree_t* plugins_node;
    
    /*!
     * Load the YAML plugin. This is required so the plugin manager can parse
     * the core plugins config file and load the core plugins. If the plugin
     * fails to load, bail.
     */
    target.name = "yaml";
    target.version.major = 0;
    target.version.minor = 0;
    target.version.patch = 1;
    g_plugin_yaml = plugin_load(game, &target, PLUGIN_VERSION_MINIMUM);
    if(!g_plugin_yaml)
        return 0;
    if(plugin_start(game, g_plugin_yaml) == PLUGIN_FAILURE)
    {
        llog(LOG_FATAL, NULL, 1, "Failed to start YAML plugin");
        return 0;
    }
    
    /*
     * Try to load and start the core plugins. If that fails, bail out.
     */
    SERVICE_CALL_NAME1(game, "yaml.load", &g_settings_doc_id, PTR(yml_settings));
    if(!g_settings_doc_id)
    {
        llog(LOG_WARNING, NULL, 3, "Config file \"", yml_settings, "\" was not found. No core plugins will be loaded");
        return 1;
    }
    SERVICE_CALL_NAME2(game, "yaml.get_node", &plugins_node, g_settings_doc_id, "plugins");
    if(!plugins_node)
    {
        llog(LOG_WARNING, NULL, 1, "Config file \"", yml_settings, "\" doesn't contain any plugins to load");
        return 1;
    }
    if(!load_plugins_from_yaml_dom(game, plugins_node))
    {
        llog(LOG_FATAL, NULL, 1, "Couldn't start all core plugins");
        return 0;
    }
    
    return 1;
}

/* -------------------------------------------------------------------------- */
char
init()
{
    /*
     * Initialise memory management first.
     */
    memory_init();
    
    /*
     * Enable logging as soon as possible
     */
    llog_init();
    
    return 1;
}

/* -------------------------------------------------------------------------- */
char
init_game(char is_server)
{
    /*
     * Create the local game server. This is the context that holds all
     * plugins, services, and events together.
     */
    g_localhost = game_create("localhost", GAME_HOST);
    if(!g_localhost)
        return 0;

    /*
     * Load and start the core plugins specified in the settings YAML file.
     */
    if(!load_core_plugins(g_localhost))
        return 0;
    
    /*
     * If we are a client, create client instance and connect to local server.
     */
    if(!is_server)
    {
        g_client = game_create("localclient", GAME_CLIENT);
        if(!g_client)
            return 0;
        
        game_connect(g_client, "localhost");
    }
    
    return 1;
}

/* -------------------------------------------------------------------------- */
void
run_game()
{
    /* 
     * Try to get the main loop service and start running the game
     */
    {
        char* start_service_name;
        struct service_t* start;
        const char* entry_point_key = "main_loop.service";

        /* search for the entry point key and retrieve its value, which is the name of the service to start with */
        SERVICE_CALL_NAME2(g_localhost, "yaml.get_value", &start_service_name, g_settings_doc_id, PTR(entry_point_key));
        if(!start_service_name)
        {
            llog(LOG_FATAL, NULL, 5, "Cannot get value of \"", entry_point_key, "\" in \"", yml_settings ,"\"");
            return;
        }

        /* with the service name retrieved, try to call it */
        start = service_get(g_localhost, start_service_name);
        if(!start)
        {
            llog(LOG_FATAL, NULL, 3, "Cannot get main loop service with name \"", start_service_name, "\"");
            return;
        }
        
        SERVICE_CALL0(start, SERVICE_NO_RETURN);
    
    }
}

/* -------------------------------------------------------------------------- */
void
deinit(void)
{
    /* clean up client */
    if(g_client)
    {
        game_destroy(g_client);
        g_client = NULL;
    }
    
    /* clean up local host */
    if(g_localhost)
    {
        SERVICE_CALL_NAME1(g_localhost, "yaml.destroy", SERVICE_NO_RETURN, g_settings_doc_id);
        game_destroy(g_localhost);
        g_localhost = NULL;
    }
    
    
    /* de-init log */
    llog_deinit();
    
    /*
     * De-init memory management last
     */
    memory_deinit();
}
