#define _SVID_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lightship/module_loader.h>
#include <lightship/plugin_manager.h>
#include <util/plugin.h>
#include <util/linked_list.h>
#include <util/string.h>

static struct list_t g_plugins;

void plugin_manager_init(void)
{
    list_init_list(&g_plugins);
}

void plugin_manager_deinit(void)
{
    /* unload all plugins */
    LIST_FOR_EACH(&g_plugins, struct plugin_t*, plugin)
    {
        plugin_unload(plugin);
    }
}

struct plugin_t* plugin_load(struct plugin_info_t* plugin_info, plugin_search_criteria_t criteria)
{
    char* filename = NULL;
    void* handle = NULL;
    struct plugin_t* plugin = NULL;
    plugin_start_func start_func;
    plugin_stop_func stop_func;
    
    /* 
     * if anything fails, the program will break from this for loop and clean
     * up any allocated memory before returning NULL. If it succeeds, the
     * plugin is returned.
     */ 
    for(;;)
    {
        /* make sure not already loaded */
        if(plugin_get_by_name(plugin_info->name))
        {
            fprintf_strings(stderr, 3, "plugin \"", plugin_info->name, "\" already loaded");
            break;
        }

        /* find a suitable file to match the criteria */
        filename = find_plugin(plugin_info, criteria);
        if(!filename)
        {
            fprintf(stderr, "Error: no such file or directory\n");
            break;
        }
        
        /* try to load library */
        handle = dlopen(filename, RTLD_LAZY);
        if(!handle)
        {
            fprintf_strings(stderr, 2, "Error loading plugin: ", dlerror());
            break;
        }
        
        /* get plugin start function */
        dlerror(); /* clear existing errors, if any */
        *(struct plugin_t**)(&start_func) = dlsym(handle, "plugin_start");
        if(!start_func)
        {
            const char* error = dlerror();
            if(error)
            {
                fprintf_strings(stderr, 2, "Error loading plugin: ", error);
                break;
            }
        }

        /* get plugin exit function */
        dlerror(); /* clear existing errors, if any */
        *(struct plugin_t**)(&stop_func) = dlsym(handle, "plugin_stop");
        if(!stop_func)
        {
            const char* error = dlerror();
            if(error)
            {
                fprintf_strings(stderr, 2, "Error loading plugin: ", error);
                break;
            }
        }

        /* start the plugin */
        plugin = plugin_create();
        start_func(plugin);
        if(!plugin)
        {
            fprintf_strings(stderr, 1, "Error starting plugin: \"plugin_start\" returned NULL");
            break;
        }
        
        /* get the version string the plugin claims to be */
        char version_str[sizeof(int)*27+1];
        plugin_get_version_string(version_str, &plugin->info);
        
        /* ensure the plugin claims to be the same version as its filename */
        if(!plugin_version_acceptable(&plugin->info, filename, PLUGIN_VERSION_EXACT))
        {
            fprintf_strings(stderr, 5,
                            "Error: plugin claims to be version ",
                            version_str,
                            ", but the filename is \"",
                            filename,
                            "\""
            );
            break;
        }
        
        /*
         * If the program has reached this point, it means the plugin has
         * successfully been loaded and passed basic verification. Copy all
         * of the relevant data into the plugin struct and return the plugin
         * object.
         */

        /* save handle and insert into list of active plugins */
        plugin->handle = handle;
        plugin->start = start_func;
        plugin->stop = stop_func;
        list_push(&g_plugins, plugin);
        
        /* print info about loaded plugin */
        fprintf_strings(stdout, 4,
            "loaded plugin \"",
            plugin->info.name,
            "\", version ",
            version_str
        );
        
        free(filename);
        return plugin;
    }
    
    /* 
     * If the program reaches this point, it means something went wrong with
     * loading the plugin. Clean up...
     */
    if(filename)
        free(filename);
    if(handle)
        dlclose(handle);
    if(plugin)
        plugin_destroy(plugin);
    
    return NULL;
}

void plugin_unload(struct plugin_t* plugin)
{
    fprintf_strings(stdout, 3, "unloading plugin \"", plugin->info.name, "\"");
    
    /* TODO notify everything that this plugin is about to be unloaded */
    
    /* shutdown plugin and clean up */
    plugin->stop();
    dlclose(plugin->handle);
    list_erase_element(&g_plugins, plugin);
    plugin_destroy(plugin);
}

struct plugin_t* plugin_get_by_name(const char* name)
{
    LIST_FOR_EACH(&g_plugins, struct plugin_t*, plugin)
    {
        if(strcmp(name, plugin->info.name) == 0)
            return plugin;
    }
    return NULL;
}

static int plugin_version_acceptable(struct plugin_info_t* info,
                        const char* file,
                        plugin_search_criteria_t criteria)
{
    uint32_t major, minor, patch;
    if(!plugin_extract_version_from_string(file, &major, &minor, &patch))
        return 0;

    /* compare version info with desired info */
    switch(criteria)
    {
        case PLUGIN_VERSION_EXACT:
            if(major == info->version.major &&
                minor == info->version.minor &&
                patch == info->version.patch)
                return 1;
            break;

        case PLUGIN_VERSION_MINIMUM:
            if(major > info->version.major)
                return 1;
            if(major == info->version.major && minor > info->version.minor)
                return 1;
            if(major == info->version.major &&
                minor == info->version.minor &&
                patch >= info->version.patch)
                return 1;
        default:
            break;
    }
    
    return 0;
}

static char* find_plugin(struct plugin_info_t* info, plugin_search_criteria_t criteria)
{
    /* log */
    char version_str[sizeof(int)*27+1];
    sprintf(version_str, "%d-%d-%d",
            info->version.major,
            info->version.minor,
            info->version.patch);
    char* crit_info[] = {"\", minimum version ", "\" exact version "};
    fprintf_strings(stdout, 4, "looking for plugin \"", info->name, crit_info[criteria],
            version_str);
    
    /* get list of files in plugins directory */
    struct dirent** namelist;
    int n = scandir("plugins", &namelist, NULL, alphasort);
    if(n < 0)
        return NULL;
    
    /* search for plugin file name */
    char* file_found = NULL;
    while(n--)
    {
        if(!file_found && 
            plugin_version_acceptable(info, namelist[n]->d_name, criteria))
        {
            file_found = malloc((strlen(namelist[n]->d_name) + 9) * sizeof(char*));
            sprintf(file_found, "plugins/%s", namelist[n]->d_name);
        }
        free(namelist[n]);
    }
    free(namelist);
    
    return file_found;
}