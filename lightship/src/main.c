#include <stdio.h>
#include <lightship/plugin_manager.h>

int main(int argc, char** argv)
{
    plugin_manager_init();
    plugin_load("../lib/libplugin_test.so");
    plugin_manager_deinit();
    
    return 0;
}