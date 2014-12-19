#include <stdio.h>
#include "lightship/plugin_manager.h"
#include "lightship/services.h"
#include "util/plugin.h"
#include "util/vector.h"

struct plugin_t* plugin_main_loop = NULL;
struct plugin_t* plugin_renderer = NULL;
struct plugin_t* plugin_input = NULL;

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
        plugin_start(plugin_main_loop);
    if(plugin_renderer)
        plugin_start(plugin_renderer);
    if(plugin_input)
        plugin_start(plugin_input);
}

int main(int argc, char** argv)
{
    plugin_manager_init();
    services_init();
    load_core_plugins();
    start_core_plugins();

    plugin_manager_deinit();
    
    return 0;
}
