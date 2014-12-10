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
    
    /* get plugin start function */
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

    /* get plugin exit function */
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
    
    /* ensure the plugin has set valid data */
    /* TODO check API version and stuff */

    /* save handle and insert into list of active plugins */
    plugin->handle = handle;
    plugin->start = start_func;
    plugin->stop = stop_func;
    list_push(&plugins, plugin);
    
    /* print info of loaded plugin */
    char version_major[sizeof(int)*8+1];
    char version_minor[sizeof(int)*8+1];
    char version_patch[sizeof(int)*8+1];
    sprintf(version_major, "%d", plugin->info.version.major);
    sprintf(version_minor, "%d", plugin->info.version.minor);
    sprintf(version_patch, "%d", plugin->info.version.patch);
    fprintf_strings(stdout, 8,
        "loaded plugin \"",
        plugin->info.name,
        "\", version ",
        version_major,
        ".",
        version_minor,
        ".",
        version_patch
    );
    
    return plugin;
}

void plugin_unload(plugin_t* plugin)
{
    fprintf_strings(stdout, 3, "unloading plugin \"", plugin->info.name, "\"...");
    plugin->stop();
    dlclose(plugin->handle);
    list_erase_element(&plugins, plugin);
}

void plugin_manager_deinit(void)
{
    /* unload all plugins */
    LIST_FOR_EACH(&plugins, plugin_t*, plugin)
    {
        plugin_unload(plugin);
    }
}