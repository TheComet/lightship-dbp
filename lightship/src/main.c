#include <stdio.h>
#include <string.h>
#include "plugin_manager/plugin_manager.h"
#include "plugin_manager/services.h"
#include "plugin_manager/events.h"
#include "plugin_manager/plugin.h"
#include "plugin_manager/game.h"
#include "util/unordered_vector.h"
#include "util/ptree.h"
#include "util/memory.h"
#include "util/log.h"
#include "util/string.h"
#include "util/pstdint.h"

#include "util/map.h"

#ifdef _DEBUG
static const char* yml_core_plugins = "../../lightship/cfg/core-plugins.yml";
static const char* yml_entry_point = "../../lightship/cfg/entry-point.yml";
#else
static const char* yml_core_plugins = "cfg/core-plugins.yml";
static const char* yml_entry_point = "cfg/entry-point.yml";
#endif

static struct plugin_t* g_plugin_yaml = NULL;
static struct game_t* g_local_game = NULL;

typedef void (*start_loop_func)(void);

void
init(void)
{
    struct plugin_info_t target;
    
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
        return;
    
    /*
     * Services and events should be initialised before anything else, as they
     * register built-in mechanics that are required throughout the rest of the
     * program (such as the log event).
     */
    services_init(g_local_game);
    events_init(g_local_game);
    
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
        return;
    if(plugin_start(g_local_game, g_plugin_yaml) == PLUGIN_FAILURE)
    {
        llog(LOG_FATAL, NULL, 1, "Failed to start YAML plugin");
        return;
    }

    /*
     * Try to load and start the core plugins. If that fails, bail out.
     */
    if(!load_plugins_from_yaml(g_local_game, yml_core_plugins))
    {
        llog(LOG_FATAL, NULL, 1, "Couldn't start all core plugins");
        return;
    }

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
        
        {
            struct menu_t;
            struct menu_t* menu;
            struct service_t* menu_load_service = service_get(g_local_game, "menu.load");
            struct service_t* menu_destroy_service = service_get(g_local_game, "menu.destroy");
#ifdef _DEBUG
            const char* menu_file_name = "../../plugins/core/menu/cfg/menu.yml";
#else
            const char* menu_file_name = "cfg/menu.yml";
#endif
            SERVICE_CALL1(menu_load_service, &menu, PTR(menu_file_name));
            SERVICE_CALL0(start, SERVICE_NO_RETURN);
            SERVICE_CALL1(menu_destroy_service, SERVICE_NO_RETURN, PTR(menu));
        }
    
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
}
#include "thread_pool/thread_pool.h"
void shit(void* p)
{
    volatile int i;
    for(i = 0; i != 10000; ++i)
    {
    }
}

int
main(int argc, char** argv)
{
    puts("=========================================");
    puts("Starting lightship");
    puts("=========================================");

    /* first thing - initialise memory management */
    memory_init();
    
    struct thread_pool_t* pool = thread_pool_create(0);
    
    int i;
    for(i = 0; i != 1000000; ++i)
        thread_pool_queue(pool, shit, NULL);

    /* initialise everything else */
    init();
    
    thread_pool_destroy(pool);

    /* clean up */
    deinit();

    /* de-init memory management and print report */
    memory_deinit();

    return 0;
}
