#include "framework/plugin_manager.h"
#include "framework/events.h"
#include "framework/services.h"
#include "framework/game.h"
#include "framework/log.h"
#include "util/memory.h"
#include "util/yaml.h"

struct ptree_t;

#ifdef _DEBUG
static const char* yml_settings = "../../lightship/cfg/settings.yml";
#else
static const char* yml_settings = "cfg/settings.yml";
#endif

static struct ptree_t* g_settings_doc;

typedef void (*start_loop_func)(void);

/* -------------------------------------------------------------------------- */
char
load_core_plugins(struct game_t* game)
{
    
    struct ptree_t* plugins_node;
    
    /*
     * Try to load and start the core plugins. If that fails, bail out.
     */
    g_settings_doc = yaml_load(yml_settings);
    if(!g_settings_doc)
    {
        llog(LOG_WARNING, game, NULL, 3, "Config file \"", yml_settings, "\" was not found. No core plugins will be loaded");
        return 1;
    }
    
    plugins_node = yaml_get_node(g_settings_doc, "plugins");
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
    yaml_init();
    
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
        yaml_destroy(g_settings_doc);
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
    yaml_deinit();
    
    /*
     * De-init memory management last
     */
    memory_deinit();
}
