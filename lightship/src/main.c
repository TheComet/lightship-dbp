#include <stdio.h>
#include <string.h>
#include "lightship/plugin_manager.h"
#include "util/services.h"
#include "util/events.h"
#include "util/plugin.h"
#include "util/unordered_vector.h"
#include "util/ptree.h"
#include "util/memory.h"
#include "util/log.h"
#include "util/string.h"
#include "util/pstdint.h"
#include "plugin_yaml.h"
#include "plugin_menu.h"
#include "plugin_renderer_gl.h"
#include "plugin_main_loop.h"

#include "util/map.h"

#ifdef _DEBUG
static const char* yml_core_plugins = "../../lightship/cfg/core-plugins.yml";
static const char* yml_entry_point = "../../lightship/cfg/entry-point.yml";
#else
static const char* yml_core_plugins = "cfg/core-plugins.yml";
static const char* yml_entry_point = "cfg/entry-point.yml";
#endif

struct plugin_t* plugin_yaml = NULL;

typedef void (*start_loop_func)(void);

void
init(void)
{
    struct plugin_info_t target;

    /*
     * Services and events should be initialised before anything else, as they
     * register built-in mechanics that are required throughout the rest of the
     * program (such as the log event).
     */
    services_init();
    events_init();
    
    /*
     * Enable logging as soon as possible (right after enabling services and
     * events)
     */
    llog_init();
    
    /*
     * The plugin manager must be initialised before being able to load
     * plugins.
     */
    plugin_manager_init();

    /*!
     * Load the YAML plugin. This is required so the plugin manager can parse
     * the core plugins config file and load the core plugins.
     */
    target.name = "yaml";
    target.version.major = 0;
    target.version.minor = 0;
    target.version.patch = 1;
    plugin_yaml = plugin_load(&target, PLUGIN_VERSION_MINIMUM);
    if(!plugin_yaml)
        return;
    if(plugin_start(plugin_yaml) == PLUGIN_FAILURE)
    {
        llog(LOG_FATAL, NULL, 1, "Failed to start YAML plugin");
        return;
    }

    /*
     * Now that the YAML plugin is loaded, plugin manager can hook in to the
     * services YAML provides.
     */
    plugin_manager_get_services();

    /*
     * Try to load and start the core plugins. If that fails, bail out.
     */
    if(!load_plugins_from_yaml(yml_core_plugins))
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
        struct service_t* yaml_load = service_get("yaml.load");
        struct service_t* yaml_get_value = service_get("yaml.get_value");
        struct service_t* yaml_destroy = service_get("yaml.destroy");
        const char* entry_point_key = "service";

        SERVICE_CALL1(yaml_load, &doc_ID, *yml_entry_point);
        if(!doc_ID)
        {
            llog(LOG_FATAL, NULL, 1, "Cannot get main loop service");
            return;
        }
        SERVICE_CALL2(yaml_get_value, &start_service_name, doc_ID, *entry_point_key);
        if(!start_service_name)
        {
            llog(LOG_FATAL, NULL, 3, "Cannot get value of \"service\" in \"", yml_entry_point ,"\"");
            SERVICE_CALL_NAME1("yaml.destroy", SERVICE_NO_RETURN, doc_ID);
            return;
        }

        start = service_get(start_service_name);
        SERVICE_CALL1(yaml_destroy, SERVICE_NO_RETURN, doc_ID);
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
    plugin_manager_deinit();
    events_deinit();
    services_deinit();
}

int
main(int argc, char** argv)
{
    puts("=========================================");
    puts("Starting lightship");
    puts("=========================================");

    /* first thing - initialise memory management */
    memory_init();

    /* initialise everything else */
    init();

    /* clean up */
    deinit();

    /* de-init memory management and print report */
    memory_deinit();

    return 0;
}
