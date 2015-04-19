#include "framework/plugin_manager.h"
#include "framework/events.h"
#include "framework/services.h"
#include "framework/game.h"
#include "framework/log.h"
#include "util/memory.h"

struct ptree_t;

#ifdef _DEBUG
static const char* yml_settings = "../../lightship/cfg/settings.yml";
#else
static const char* yml_settings = "cfg/settings.yml";
#endif

static struct plugin_t* g_plugin_yaml = NULL;
static uint32_t g_settings_doc_id;

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
        llog(LOG_FATAL, game, NULL, 1, "Failed to start YAML plugin");
        return 0;
    }
    
    /*
     * Try to load and start the core plugins. If that fails, bail out.
     */
    SERVICE_CALL_NAME1(game, "yaml.load", &g_settings_doc_id, PTR(yml_settings));
    if(!g_settings_doc_id)
    {
        llog(LOG_WARNING, game, NULL, 3, "Config file \"", yml_settings, "\" was not found. No core plugins will be loaded");
        return 1;
    }
    SERVICE_CALL_NAME2(game, "yaml.get_node", &plugins_node, g_settings_doc_id, "plugins");
    if(!plugins_node)
    {
        llog(LOG_WARNING, game, NULL, 1, "Config file \"", yml_settings, "\" doesn't contain any plugins to load");
        return 1;
    }
    if(!load_plugins_from_yaml_dom(game, plugins_node))
    {
        llog(LOG_FATAL, game, NULL, 1, "Couldn't start all core plugins");
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
     * Initialise global stuff.
     */
    
    return 1;
}

/* -------------------------------------------------------------------------- */
char
init_game(char is_server)
{
    struct game_t *client, *localhost;
    
    /*
     * Create the local game server. This is the context that holds all
     * plugins, services, and events together.
     */
    localhost = game_create("localhost", GAME_HOST);
    if(!localhost)
        return 0;

    /*
     * Load and start the core plugins specified in the settings YAML file.
     */
    if(!load_core_plugins(localhost))
    {
        SERVICE_CALL_NAME1(localhost, "yaml.destroy", SERVICE_NO_RETURN, g_settings_doc_id);
        game_destroy(localhost);
        localhost = NULL;
        return 0;
    }
    
    /* TODO: remove - for now, to bootstrap the menu */
    {
        char* menu_file_name;
        struct menu_t;
        struct menu_t* menu;
#ifdef _DEBUG
        menu_file_name = "../../plugins/core/menu/cfg/menu.yml";
#else
        menu_file_name = "cfg/menu.yml";
#endif
        SERVICE_CALL_NAME1(localhost, "menu.load", &menu, PTR(menu_file_name));
    }
    
    /*
     * If we are a client, create client instance and connect to local server.
     */
    if(!is_server)
    {
        client = game_create("localclient", GAME_CLIENT);
        if(!client)
            return 0;
        
        game_connect(client, "localhost");
    }
    
    return 1;
}

/* -------------------------------------------------------------------------- */
void
deinit(void)
{
    /*
     * De-init global stuff
     */
    
    /*
     * De-init memory management last
     */
    memory_deinit();
}
