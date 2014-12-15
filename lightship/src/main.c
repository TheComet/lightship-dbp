#include <stdio.h>
#include <lightship/plugin_manager.h>
#include <util/plugin.h>
#include <util/vector.h>

struct plugin_t* plugin_test = NULL;
struct plugin_t* plugin_renderer = NULL;

void load_core_plugins(void)
{
    struct plugin_info_t target;
    
    /* load a test plugin */
    target.name = "test";
    target.version.major = 0;
    target.version.minor = 0;
    target.version.patch = 0;
    plugin_test = plugin_load(&target, PLUGIN_VERSION_MINIMUM);
    
    /* load graphics plugin */
    target.name = "renderer_gl";
    target.version.major = 0;
    target.version.minor = 0;
    target.version.patch = 1;
    plugin_renderer = plugin_load(&target, PLUGIN_VERSION_MINIMUM);
}

void start_core_plugins(void)
{
    if(plugin_test)
        plugin_start(plugin_test);
    if(plugin_renderer)
        plugin_start(plugin_renderer);
}

void unload_core_plugins(void)
{
    /* unload plugins */
    if(plugin_test)
        plugin_unload(plugin_test);
    if(plugin_renderer)
        plugin_unload(plugin_renderer);
}

int main(int argc, char** argv)
{
    plugin_manager_init();
    load_core_plugins();
    start_core_plugins();

    unload_core_plugins();
    plugin_manager_deinit();
    
    return 0;
}
