#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lightship/plugin_manager.h>
#include <util/config.h>
#include <util/plugin.h>
#include <util/linked_list.h>
#include <util/string.h>
#include <util/module_loader.h>
#include <util/dir.h>

static struct list_t g_plugins;

void plugin_manager_init(void)
{
    list_init_list(&g_plugins);
}

void plugin_manager_deinit(void)
{
    /* unload all plugins */
    LIST_FOR_EACH(&g_plugins, struct plugin_t, plugin)
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
    char version_str[sizeof(int)*27+1];
    
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
        handle = module_open(filename);
        if(!handle)
            break;

        /* get plugin start function */
        *(struct plugin_t**)(&start_func) = module_sym(handle, "plugin_start");
        if(!start_func)
            break;

        /* get plugin exit function */
        *(void**)(&stop_func) = module_sym(handle, "plugin_stop");
        if(!stop_func)
            break;

        /* start the plugin */
        plugin = start_func();
        if(!plugin)
        {
            fprintf_strings(stderr, 1, "Error starting plugin: \"plugin_start\" returned NULL");
            break;
        }
        
        /* get the version string the plugin claims to be */
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
        module_close(handle);
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
    module_close(plugin->handle);
    list_erase_element(&g_plugins, plugin);
}

struct plugin_t* plugin_get_by_name(const char* name)
{
    LIST_FOR_EACH(&g_plugins, struct plugin_t, plugin)
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
    int n;
    char version_str[sizeof(int)*27+1];
    struct list_t* list;

    /* local variables */
    const char* crit_info[] = {"\", minimum version ", "\" exact version "};
    char* file_found = NULL;

    /* log */
    sprintf(version_str, "%d-%d-%d",
            info->version.major,
            info->version.minor,
            info->version.patch);
    fprintf_strings(stdout, 4, "looking for plugin \"", info->name, crit_info[criteria],
            version_str);
    
    /* 
     * Get list of files in various plugin directories.
     * TODO load from config file
     */
    list = list_create();
#ifdef LIGHTSHIP_PLATFORM_LINUX
    get_directory_listing(list, "plugins/");
#elif defined(LIGHTSHIP_PLATFORM_WINDOWS)
#    ifdef _DEBUG
    get_directory_listing(list, "..\\bin\\Debug\\");
#    else
    get_directory_listing(list, "..\\bin\\Release\\");
#    endif
#elif defined(LIGHTSHIP_PLATFORM_MAC)
    get_directory_listing(list, "plugins/");
#endif

    /* search for plugin file name matching criteria */
    { /* need these braces because LIST_FOR_EACH declares new variables */
        LIST_FOR_EACH(list, char, name)
        {
            if(!file_found && 
                plugin_version_acceptable(info, name, criteria))
            {
                file_found = name;
            }
            else
            {
                /* can also free the strings no longer needed */
                free(name);
            }
        }
    }
    
    /* free list of directories */
    list_destroy(list);
    
    return file_found;
}
