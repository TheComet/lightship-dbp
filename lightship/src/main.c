#include <stdio.h>
#include <lightship/plugin_manager.h>
#include <util/plugin.h>
#include <util/vector.h>

struct action_t;
typedef void (*callback_func)(void);
typedef void (*signal_func)(struct action_t*);

struct signal_t
{
    signal_func fire;
};

struct action_t
{
    callback_func exec;
};



int main(int argc, char** argv)
{
    struct plugin_t* plugin;
    struct plugin_info_t target;

    plugin_manager_init();
    
    /* load a test plugin */
    target.name = "test";
    target.version.major = 0;
    target.version.minor = 0;
    target.version.patch = 0;
    plugin = plugin_load(&target, PLUGIN_VERSION_MINIMUM);
    
    /* unload the test plugin */
    if(plugin)
        plugin_unload(plugin);

    plugin_manager_deinit();
    
    return 0;
}
