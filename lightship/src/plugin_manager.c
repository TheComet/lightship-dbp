#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lightship/plugin_manager.h"
#include "plugin_yaml/services.h"
#include "util/services.h"
#include "util/events.h"
#include "util/config.h"
#include "util/plugin.h"
#include "util/linked_list.h"
#include "util/unordered_vector.h"
#include "util/ptree.h"
#include "util/string.h"
#include "util/module_loader.h"
#include "util/dir.h"
#include "util/memory.h"
#include "util/log.h"

static struct list_t g_plugins;

static yaml_load_func yaml_load;
static yaml_destroy_func yaml_destroy;
static yaml_get_dom_func yaml_get_dom;
static yaml_get_value_func yaml_get_value;

/*!
 * @brief Evaluates whether the specified file is an acceptable plugin to load
 * based on the specified info and criteria.
 * @param[in] info The requested plugin to try and match.
 * @param[in] file The file to test.
 * @param[in] criteria The criteria to use.
 * @return Returns 1 if successful, 0 if otherwise.
 */
static char
plugin_version_acceptable(const struct plugin_info_t* info,
                          const char* file,
                          const plugin_search_criteria_t criteria);

/*!
 * @brief Scans the plugin directory for a suitable plugin to load.
 * @param[in] info The requested plugin to try and match.
 * @param[in] criteria The criteria to use.
 * @return Returns the full file name and relative path if a plugin was
 * matched. Returns NULL on failure.
 */
static char*
find_plugin(const struct plugin_info_t* info,
            const plugin_search_criteria_t criteria);

void
plugin_manager_init(void)
{
    list_init_list(&g_plugins);
}

void
plugin_manager_get_services(void)
{
    yaml_load = (yaml_load_func)service_get("yaml.load");
    yaml_destroy = (yaml_destroy_func)service_get("yaml.destroy");
    yaml_get_dom = (yaml_get_dom_func)service_get("yaml.get_dom");
    yaml_get_value = (yaml_get_value_func)service_get("yaml.get_value");
}

void
plugin_manager_deinit(void)
{
    /* unload all plugins */
    LIST_FOR_EACH_ERASE_R(&g_plugins, struct plugin_t, plugin)
    {
        /* NOTE this erases the plugin object from the linked list */
        plugin_unload(plugin);
    }
}

struct plugin_t*
plugin_load(const struct plugin_info_t* plugin_info,
            const plugin_search_criteria_t criteria)
{
    /* will contain the file name of the plugin if it is found. Must be FREE()'d */
    char* filename = NULL;
    /* will hold the handle of the loaded module, if successful */
    void* handle = NULL;
    /* functions to extract from the loaded module */
    plugin_init_func init_func;
    plugin_start_func start_func;
    plugin_stop_func stop_func;
    plugin_deinit_func deinit_func;
    /* will reference the allocated plugin object returned by plugin_init() */
    struct plugin_t* plugin = NULL;
    /* buffer for holding the various version strings that will be constructed */
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
            llog(LOG_ERROR, 3, "plugin \"", plugin_info->name, "\" already loaded");
            break;
        }

        /* find a suitable file to match the criteria */
        filename = find_plugin(plugin_info, criteria);
        if(!filename)
        {
            llog(LOG_ERROR, 1, "Error searching for plugin: Unable to find a file matching the critera");
            break;
        }
        
        /* try to load library */
        handle = module_open(filename);
        if(!handle)
            break;
        
        /* get plugin initialise function */
        *(void**)(&init_func) = module_sym(handle, "plugin_init");
        if(!init_func)
            break;

        /* get plugin start function */
        *(void**)(&start_func) = module_sym(handle, "plugin_start");
        if(!start_func)
            break;

        /* get plugin exit function */
        *(void**)(&stop_func) = module_sym(handle, "plugin_stop");
        if(!stop_func)
            break;
        
        /* get plugin deinit function */
        *(void**)(&deinit_func) = module_sym(handle, "plugin_deinit");
        if(!deinit_func)
            break;

        /* initialise the plugin */
        plugin = init_func();
        if(!plugin)
        {
            llog(LOG_ERROR, 1, "Error initialising plugin: \"plugin_init\" returned NULL");
            break;
        }
        
        /* get the version string the plugin claims to be */
        plugin_get_version_string(version_str, &plugin->info);
        
        /* ensure the plugin claims to be the same version as its filename */
        if(!plugin_version_acceptable(&plugin->info, filename, PLUGIN_VERSION_EXACT))
        {
            llog(LOG_ERROR, 5,
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
         * of the relevant data into the plugin struct, push it into the list
         * of loaded plugins and return the plugin object.
         */

        plugin->handle = handle;
        plugin->init = init_func;
        plugin->start = start_func;
        plugin->stop = stop_func;
        plugin->deinit = deinit_func;
        list_push(&g_plugins, plugin);
        
        /* print info about loaded plugin */
        llog(LOG_INFO, 4,
            "loaded plugin \"",
            plugin->info.name,
            "\", version ",
            version_str
        );
        
        FREE(filename);
        return plugin;
    }
    
    /* 
     * If the program reaches this point, it means something went wrong with
     * loading the plugin. Clean up...
     */
    if(filename)
        FREE(filename);
    if(handle)
        module_close(handle);
    if(plugin)
        plugin_destroy(plugin);
    
    return NULL;
}

char
load_plugins_from_yaml(const char* filename)
{
    struct plugin_info_t target;
    struct ptree_t* dom;
    struct unordered_vector_t new_plugins;
    uint32_t doc_ID;
	char success = 1;
    
    unordered_vector_init_vector(&new_plugins, sizeof(struct plugin_t*));

    /* try to load YAML file */
    doc_ID = ((yaml_load_func)service_get("yaml.load"))(filename);
    if(!doc_ID)
        return 0;
    
    /* get DOM and get_value service */
    dom = yaml_get_dom(doc_ID);
    if(!dom)
    {
        llog(LOG_FATAL, 1, "Failed to get DOM");
        yaml_destroy(doc_ID);
        return 0;
    }
    
    /* load all plugins in DOM */
    {
        UNORDERED_VECTOR_FOR_EACH(&dom->children, struct ptree_t, child)
        {
            char* name;
            char* version;
            char* policy;
            struct plugin_t* plugin;
            plugin_search_criteria_t criteria;
            
            /* extract information from tree */
            name = ptree_find_by_key(child, "name");
            version = ptree_find_by_key(child, "version");
            policy = ptree_find_by_key(child, "version_policy");
            if(!name)
            {
                llog(LOG_ERROR, 1, "Key \"name\" isn't defined for plugin");
                continue;
            }
            if(!version)
            {
                llog(LOG_ERROR, 1, "Key \"version\" isn't defined for plugin");
                continue;
            }
            if(!policy)
            {
                llog(LOG_WARNING, 1, "Key \"version_policy\" isn't defined for plugin. Using default \"minimum\"");
                policy = "minimum";
            }
            target.name = name;
            if(!plugin_extract_version_from_string(version,
                                                &target.version.major,
                                                &target.version.minor,
                                                &target.version.patch))
            {
                llog(LOG_ERROR, 1, "Version string of plugin \"", name, "\" is invalid. Should be major.minor.patch");
                continue;
            }
            if(strncmp("minimum", policy, 7) == 0)
                criteria = PLUGIN_VERSION_MINIMUM;
            else if(strncmp("exact", policy, 5) == 0)
                criteria = PLUGIN_VERSION_EXACT;
            else
            {
                llog(LOG_ERROR, 3, "Invalid policy \"", policy, "\"");
                continue;
            }
            
            /* load plugin */
            plugin = plugin_load(&target, criteria);
            if(!plugin)
                continue;
            unordered_vector_push(&new_plugins, &plugin);
        }
    }

    /* start plugins */
    {
        UNORDERED_VECTOR_FOR_EACH(&new_plugins, struct plugin_t*, pluginp)
        {
            if(!plugin_start(*pluginp))
            {
                llog(LOG_ERROR, 3, "Failed to start plugin \"", (*pluginp)->info.name, "\", unloading...");
                plugin_unload(*pluginp);
                success = 0;
            }
        }
    }

    /* clean up */
    yaml_destroy(doc_ID);
    unordered_vector_clear_free(&new_plugins);
    return success;
}

void
plugin_unload(struct plugin_t* plugin)
{
    void* module_handle;
    llog(LOG_INFO, 3, "unloading plugin \"", plugin->info.name, "\"");
    
    /* TODO notify everything that this plugin is about to be unloaded */

    /* unregister all services and events registered by this plugin */
    service_unregister_all(plugin);
    event_destroy_all_plugin_events(plugin);
    event_unregister_all_listeners_of_plugin(plugin);
    
    /* stop the plugin */
    plugin_stop(plugin);

    /* 
     * NOTE The plugin object becomes invalid as soon as plugin_deinit() is
     * called. That is why the module handle must first be extracted before
     * stopping the plugin, so we can unload the module safely.
     * NOTE Erasing an element from the linked list does not require the plugin
     * object to be valid, as it does not dereference it.
     */
    module_handle = plugin->handle;
    plugin_deinit(plugin);
    module_close(module_handle);
    list_erase_element(&g_plugins, plugin);
}

struct plugin_t*
plugin_get_by_name(const char* name)
{
    LIST_FOR_EACH(&g_plugins, struct plugin_t, plugin)
    {
        if(strcmp(name, plugin->info.name) == 0)
            return plugin;
    }
    return NULL;
}

static char
plugin_version_acceptable(const struct plugin_info_t* info,
                          const char* file,
                          const plugin_search_criteria_t criteria)
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

static char*
find_plugin(const struct plugin_info_t* info,
            const plugin_search_criteria_t criteria)
{
    /* local variables */
    char version_str[sizeof(int)*27+1];
    struct list_t* list;
    const char* crit_info[] = {"\", minimum version ", "\" exact version "};
    char* file_found = NULL;

    /* log */
    sprintf(version_str, "%d.%d.%d",
            info->version.major,
            info->version.minor,
            info->version.patch);
    llog(LOG_INFO, 4, "looking for plugin \"", info->name, crit_info[criteria],
            version_str);
    
    /* 
     * Get list of files in various plugin directories.
     */
    list = list_create();
#if defined(LIGHTSHIP_PLATFORM_WINDOWS)
    get_directory_listing(list, "plugins\\");
#else
    get_directory_listing(list, "plugins/");
#endif 

    /* search for plugin file name matching criteria */
    { /* need these braces because LIST_FOR_EACH declares new variables */
        LIST_FOR_EACH(list, char, name)
        {
            if(!file_found && 
                strstr(name, info->name) &&
                plugin_version_acceptable(info, name, criteria))
            {
                file_found = name;
            }
            else
            {
                /* 
                 * get_directory_listing() allocates the strings it pushes into
                 * the linked list, and it is up to us to FREE them.
                 */
                FREE(name);
            }
        }
    }
    
    /* FREE list of directories */
    list_destroy(list);
    
    return file_found;
}
