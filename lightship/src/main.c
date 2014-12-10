#include <stdio.h>
#include <lightship/plugin_manager.h>
#include <lightship/plugin.h>

int main(int argc, char** argv)
{
    plugin_manager_init();
    plugin_t* plugin = plugin_load("../lib/libplugin_test.so");
    plugin_unload(plugin);
    plugin_manager_deinit();
    
    return 0;
}