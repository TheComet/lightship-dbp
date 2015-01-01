#include <stdio.h>
#include <string.h>
#include "lightship/plugin_manager.h"
#include "util/services.h"
#include "util/events.h"
#include "lightship/api.h"
#include "util/plugin.h"
#include "util/unordered_vector.h"
#include "util/ptree.h"
#include "util/memory.h"
#include "util/log.h"

struct plugin_t* plugin_yaml = NULL;

typedef void (*start_loop_func)(void);
typedef uint32_t (*yaml_load_func)(const char*);
typedef struct ptree_t* (*yaml_get_dom_func)(uint32_t);
typedef char* (*yaml_get_value_func)(uint32_t, const char*);
typedef void (*yaml_destroy_func)(uint32_t);

char
load_core_plugins(void)
{
    struct plugin_info_t target;
    struct ptree_t* dom;
    struct unordered_vector_t new_plugins;
    uint32_t doc_ID;
    
    unordered_vector_init_vector(&new_plugins, sizeof(struct plugin_t*));

    /* try to load YAML file */
    doc_ID = ((yaml_load_func)service_get("yaml.load"))("cfg/core-plugins.yml");
    if(!doc_ID)
    {
        llog(LOG_FATAL, 1, "Cannot load core plugins without config file.");
        return 0;
    }
    
    /* get DOM and get_value service */
    dom = ((yaml_get_dom_func)service_get("yaml.get_dom"))(doc_ID);
    if(!dom)
    {
        llog(LOG_FATAL, 1, "Failed to get DOM");
        return 0;
    }
    
    /* load all plugins in DOM */
    {
        UNORDERED_VECTOR_FOR_EACH(&dom->children, struct ptree_t, child)
        {
            char* name;
            char* version;
            char* policy;
            struct plugin_t* plugin;
            plugin_search_criteria_t criteria;
            
            /* extract information from tree */
            name = ptree_find_by_key(child, "name");
            version = ptree_find_by_key(child, "version");
            policy = ptree_find_by_key(child, "version_policy");
            if(!name)
            {
                llog(LOG_ERROR, 1, "Key \"name\" isn't defined for plugin");
                continue;
            }
            if(!version)
            {
                llog(LOG_ERROR, 1, "Key \"version\" isn't defined for plugin");
                continue;
            }
            if(!policy)
            {
                llog(LOG_WARNING, 1, "Key \"version_policy\" isn't defined for plugin");
                policy = "minimum";
            }
            target.name = name;
            if(!plugin_extract_version_from_string(version,
                                                &target.version.major,
                                                &target.version.minor,
                                                &target.version.patch))
            {
                llog(LOG_ERROR, 1, "Version string is invalid. Should be major.minor.patch");
                continue;
            }
            if(strncmp("minimum", policy, 7) == 0)
                criteria = PLUGIN_VERSION_MINIMUM;
            else if(strncmp("exact", policy, 5) == 0)
                criteria = PLUGIN_VERSION_EXACT;
            else
            {
                llog(LOG_ERROR, 3, "Invalid policy \"", policy, "\"");
                continue;
            }
            
            /* load plugin */
            plugin = plugin_load(&target, criteria);
            if(!plugin)
                continue;
            unordered_vector_push(&new_plugins, &plugin);
        }
    }

    /* start plugins */
    {
        UNORDERED_VECTOR_FOR_EACH(&new_plugins, struct plugin_t*, pluginp)
        {
            if(!plugin_start(*pluginp))
            {
                plugin_unload(*pluginp);
            }
        }
    }

    /* clean up */
    ((yaml_destroy_func)service_get("yaml.destroy"))(doc_ID);
    unordered_vector_clear_free(&new_plugins);
    return 1;
}

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
     * The lightship API must be initialised before any plugins can be
     * loaded, so an api struct can be passed to each plugin.
     */
    api_init();
    
    /*
     * The plugin manager must be initialised before being able to load
     * plugins.
     */
    plugin_manager_init();

    /*!
     * Load the YAML plugin. This is required to parse the core plugins file
     * to load the core plugins.
     */
    target.name = "yaml";
    target.version.major = 0;
    target.version.minor = 0;
    target.version.patch = 1;
    plugin_yaml = plugin_load(&target, PLUGIN_VERSION_MINIMUM);
    if(!plugin_yaml)
        return;
    if(plugin_start(plugin_yaml) == PLUGIN_FAILURE)
        return;

    /*
     * Try to load and start the core plugins. If that fails, bail out.
     */
    if(!load_core_plugins())
        return;
    
    typedef uint32_t (*button_create_func)(const char*, float, float, float, float);
    uint32_t id = ((button_create_func)service_get("menu.button_create"))("test", 0.0, 0.0, 0.3, 0.1);

    /* 
     * Try to get the main loop service and start running the game
     */
    {
        char* start_service_name;
        start_loop_func start;
        uint32_t doc_ID = ((yaml_load_func)service_get("yaml.load"))("cfg/entry-point.yml");
        if(!doc_ID)
        {
            llog(LOG_FATAL, 1, "Cannot get main loop service");
            return;
        }
        start_service_name = ((yaml_get_value_func)service_get("yaml.get_value"))(doc_ID, "service");
        if(!start_service_name)
        {
            llog(LOG_FATAL, 1, "Cannot get value of \"service\" in \"cfg/entry-poing.yml\"");
            ((yaml_destroy_func)service_get("yaml.destroy"))(doc_ID);
            return;
        }
            

        start = (start_loop_func)service_get(start_service_name);
        ((yaml_destroy_func)service_get("yaml.destroy"))(doc_ID);
        if(!start)
        {
            llog(LOG_FATAL, 3, "Cannot get main loop service \"", start_service_name, "\"");
            return;
        }

        start();
    }
}

void
deinit(void)
{
    plugin_manager_deinit();
    events_deinit();
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
