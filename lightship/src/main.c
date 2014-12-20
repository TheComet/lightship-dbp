#include <stdio.h>
#include "lightship/plugin_manager.h"
#include "lightship/services.h"
#include "lightship/events.h"
#include "lightship/api.h"
#include "util/plugin.h"
#include "util/vector.h"

struct plugin_t* plugin_main_loop = NULL;
struct plugin_t* plugin_renderer = NULL;
struct plugin_t* plugin_input = NULL;

typedef void (*start_loop_func)(void); start_loop_func start;
typedef void (*stop_loop_func) (void); stop_loop_func stop;

void load_core_plugins(void)
{
    struct plugin_info_t target;
    
    /* load main loop plugin */
    target.name = "main_loop";
    target.version.major = 0;
    target.version.minor = 0;
    target.version.patch = 1;
    plugin_main_loop = plugin_load(&target, PLUGIN_VERSION_MINIMUM);
    
    /* load graphics plugin */
    target.name = "renderer_gl";
    target.version.major = 0;
    target.version.minor = 0;
    target.version.patch = 1;
    plugin_renderer = plugin_load(&target, PLUGIN_VERSION_MINIMUM);
    
    /* load input plugin */
    target.name = "input";
    target.version.major = 0;
    target.version.minor = 0;
    target.version.patch = 1;
    plugin_input = plugin_load(&target, PLUGIN_VERSION_MINIMUM);
}

void start_core_plugins(void)
{
    if(plugin_main_loop)
        if(plugin_start(plugin_main_loop) == PLUGIN_FAILURE)
            return;
    if(plugin_renderer)
        if(plugin_start(plugin_renderer) == PLUGIN_FAILURE)
            return;
    if(plugin_input)
        if(plugin_start(plugin_input) == PLUGIN_FAILURE)
            return;
}

int main(int argc, char** argv)
{
    api_init();
    services_init();
    events_init();
    plugin_manager_init();

    load_core_plugins();
    start_core_plugins();
    
    start = (start_loop_func)service_get("main_loop.start");
    stop  = (stop_loop_func) service_get("main_loop.stop");

    plugin_manager_deinit();

    return 0;
}
