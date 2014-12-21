#include <stdio.h>
#include "lightship/plugin_manager.h"
#include "lightship/services.h"
#include "lightship/events.h"
#include "lightship/api.h"
#include "util/plugin.h"
#include "util/vector.h"
#include "util/memory.h"

struct plugin_t* plugin_main_loop = NULL;
struct plugin_t* plugin_renderer = NULL;
struct plugin_t* plugin_input = NULL;

typedef void (*start_loop_func)(void); start_loop_func start;

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

char start_core_plugins(void)
{
    if(!plugin_main_loop || plugin_start(plugin_main_loop) == PLUGIN_FAILURE)
        return 0;
    if(!plugin_renderer  || plugin_start(plugin_renderer) == PLUGIN_FAILURE)
        return 0;
    if(!plugin_input     || plugin_start(plugin_input) == PLUGIN_FAILURE)
        return 0;

    return 1;
}

char init(void)
{
    api_init();
    services_init();
    events_init();
    plugin_manager_init();

    load_core_plugins();
    if(!start_core_plugins())
        return 0;
    
    return 1;
}

void deinit(void)
{
    plugin_manager_deinit();
}

int main(int argc, char** argv)
{
    /* initialise everything */
    if(!init())
    {
        deinit();
        return 0;
    }

    /* get start of main loop and enter */
    start = (start_loop_func)service_get("main_loop.start");
    if(start)
        start();
    else
        fprintf(stderr, "Failed to find main loop \"start\" function");

    /* clean up */
    deinit();
    memory_report();

    return 0;
}
