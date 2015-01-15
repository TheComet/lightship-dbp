#include <stdio.h>
#include <string.h>
#include "lightship/plugin_manager.h"
#include "lightship/api.h"
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
    {
        llog(LOG_FATAL, 1, "Failed to start YAML plugin");
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
        return;
    {
        button_create_func button_create = ((button_create_func)service_get("menu.button_create"));
        text_load_font_func text_load_font = ((text_load_font_func)service_get("renderer_gl.text_load_font"));
        text_load_characters_func text_load_characters = ((text_load_characters_func)service_get("renderer_gl.text_load_characters"));
        text_add_static_center_string_func text_add_static_center_string = ((text_add_static_center_string_func)service_get("renderer_gl.text_add_static_center_string"));
        

        
        uint32_t font = text_load_font("../../plugins/core/menu/ttf/DejaVuSans.ttf", 9);
        text_load_characters(font, NULL);
        button_create("test", 0.0, 0.0, 0.3, 0.1);
        text_add_static_center_string(font, 0, 0, L"test");
        button_create("test", 0.0, 0.0, 0.3, 0.1);
        button_create("test", 0.0, 0.0, 0.3, 0.1);
        text_add_static_center_string(font, 0, 0, L"test");

    }
    /* 
     * Try to get the main loop service and start running the game
     */
    {
        char* start_service_name;
        start_loop_func start;
        uint32_t doc_ID = ((yaml_load_func)service_get("yaml.load"))(yml_entry_point);
        if(!doc_ID)
        {
            llog(LOG_FATAL, 1, "Cannot get main loop service");
            return;
        }
        start_service_name = ((yaml_get_value_func)service_get("yaml.get_value"))(doc_ID, "service");
        if(!start_service_name)
        {
            llog(LOG_FATAL, 3, "Cannot get value of \"service\" in \"", yml_entry_point ,"\"");
            ((yaml_destroy_func)service_get("yaml.destroy"))(doc_ID);
            return;
        }

        start = (start_loop_func)service_get(start_service_name);
        ((yaml_destroy_func)service_get("yaml.destroy"))(doc_ID);
        if(!start)
        {
            llog(LOG_FATAL, 1, "Cannot get main loop service");
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
