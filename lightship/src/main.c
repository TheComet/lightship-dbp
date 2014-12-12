#include <stdio.h>
#include <lightship/plugin_manager.h>
#include <util/plugin.h>

struct
{
    int fuck;
} gay;

int main(int argc, char** argv)
{
    plugin_manager_init();
    
    /* load a test plugin */
    plugin_info_t target;
    target.name = "test";
    target.version.major = 0;
    target.version.minor = 0;
    target.version.patch = 0;
    plugin_t* plugin = plugin_load(&target, PLUGIN_VERSION_MINIMUM);
    
    /* unload the test plugin */
    if(plugin)
        plugin_unload(plugin);

    plugin_manager_deinit();
    
    return 0;
}
