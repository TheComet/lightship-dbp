#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lightship/plugin_manager.h>
#include <lightship/plugin.h>
#include <lightship/linked_list.h>
#include <lightship/utility.h>

#include <dlfcn.h>

static list_t plugins;

void plugin_manager_init(void)
{
    list_init_list(&plugins);
}

plugin_t* plugin_load(const char* filename)
{
    /* try to load library */
    fprintf_strings(stdout, 3, "loading plugin \"", filename, "\"...");
    void* handle = dlopen(filename, RTLD_LAZY);
    if(!handle)
    {
        fprintf_strings(stderr, 2, "Error loading plugin: ", dlerror());
        return NULL;
    }
    
    /* get plugin init function */
    plugin_start_func start_func;
    *(plugin_t**)(&start_func) = dlsym(handle, "plugin_start");
    if(!start_func)
    {
        const char* error = dlerror();
        if(error)
        {
            fprintf_strings(stderr, 2, "Error loading plugin: ", error);
            dlclose(handle);
            return NULL;
        }
    }

    /* make sure plugin has exit function */
    plugin_stop_func stop_func;
    *(plugin_t**)(&stop_func) = dlsym(handle, "plugin_stop");
    if(!stop_func)
    {
        const char* error = dlerror();
        if(error)
        {
            fprintf_strings(stderr, 2, "Error loading plugin: ", error);
            dlclose(handle);
            return NULL;
        }
    }

    /* start the plugin */
    plugin_t* plugin = start_func();
    if(!plugin)
    {
        fprintf_strings(stderr, 1, "Error starting plugin: \"plugin_start\" returned NULL");
        dlclose(handle);
        return NULL;
    }

    /* save handle and insert into list of active plugins */
    plugin->handle = handle;
    plugin->start = start_func;
    plugin->stop = stop_func;
    list_push(&plugins, plugin);
    
    return plugin;
}

void plugin_unload(plugin_t* plugin)
{
    fprintf_strings(stdout, 3, "unloading plugin \"", plugin->info.name, "\"...");
    plugin->stop();
    dlclose(plugin->handle);
}

void plugin_manager_deinit(void)
{
    /* unload all plugins */
    plugin_t* plugin;
    LIST_FOR_EACH(&plugins, plugin)
    {
        plugin_unload(plugin);
        list_erase_node(&plugins, node);
    }
}