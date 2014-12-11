#define _SVID_SOURCE
#include <dlfcn.h>
#include <dirent.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lightship/plugin_manager.h>
#include <lightship/plugin.h>
#include <lightship/linked_list.h>
#include <util/util.h>

static list_t plugins;

void plugin_manager_init(void)
{
    list_init_list(&plugins);
}

void plugin_manager_deinit(void)
{
    /* unload all plugins */
    LIST_FOR_EACH(&plugins, plugin_t*, plugin)
    {
        plugin_unload(plugin);
    }
}

plugin_t* plugin_load(plugin_info_t* plugin_info, plugin_search_criteria_t criteria)
{
    /* make sure not already loaded */
    if(plugin_get_by_name(plugin_info->name))
    {
        fprintf_strings(stderr, 3, "plugin \"", plugin_info->name, "\" already loaded");
        return NULL;
    }

    /* try to load library */
    char* filename = find_plugin(plugin_info, criteria);
    if(!filename)
    {
        fprintf(stderr, "Error: no such file or directory\n");
        return NULL;
    }
    void* handle = dlopen(filename, RTLD_LAZY);
    free(filename);
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
    
    /* print info about loaded plugin */
    char version_str[sizeof(int)*27+1];
    sprintf(version_str, "%d-%d-%d",
            plugin->info.version.major,
            plugin->info.version.minor,
            plugin->info.version.patch);
    fprintf_strings(stdout, 4,
        "loaded plugin \"",
        plugin->info.name,
        "\", version ",
        version_str
    );
    
    return plugin;
}

void plugin_unload(plugin_t* plugin)
{
    fprintf_strings(stdout, 3, "unloading plugin \"", plugin->info.name, "\"");
    plugin->stop();
    dlclose(plugin->handle);
    list_erase_element(&plugins, plugin);
}

plugin_t* plugin_get_by_name(const char* name)
{
    LIST_FOR_EACH(&plugins, plugin_t*, plugin)
    {
        if(strcmp(name, plugin->info.name) == 0)
            return plugin;
    }
    return NULL;
}

static int plugin_extract_version_from_string(const char* file,
                                       uint32_t* major,
                                       uint32_t* minor,
                                       uint32_t* patch)
{
    /* strtok modifies the character array, copy into temporary */
    char file_temp[(strlen(file)+1)*sizeof(char*)];
    strcpy(file_temp, file);
    
    /* extract major, minor, and patch from file name */
    *major = -1;
    *minor = -1;
    *patch = -1;
    char* pch = strtok(file_temp, "-");
    /* skip ahead until a token is found that contains a number */
    while(pch != NULL)
    {
        if(strpbrk(pch, "0123456789") != NULL)
            break;
        pch = strtok(NULL, "-");
    }
    /* the following numbers must be major, minor, and patch numbers */
    if(pch != NULL)
        *major = atoi(pch);
    if((pch = strtok(NULL, "-")) != NULL)
        *minor = atoi(pch);
    if((pch = strtok(NULL, "-")) != NULL)
        *patch = atoi(pch);
    
    /* error check */
    if(*major == -1 || *minor == -1 || *patch == -1)
        return 0;

    return 1;
}

static int plugin_version_acceptable(plugin_info_t* info,
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

static char* find_plugin(plugin_info_t* info, plugin_search_criteria_t criteria)
{
    /* 
     * Construct plugin file name from plugin name.
     * File name follows the pattern:
     *   plugin_(name)-major-minor-patch.(extension)
     */
    char version_str[sizeof(int)*27+1];
    sprintf(version_str, "%d-%d-%d",
            info->version.major,
            info->version.minor,
            info->version.patch);
    char filename[(12 + strlen(info->name) + strlen(version_str)) * sizeof(char*)];
    sprintf(filename, "plugin_%s-%s.so", info->name, version_str);
    
    /* log */
    fprintf_strings(stdout, 4,
            "looking for plugin \"",
            info->name,
            "\", minimum version ",
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