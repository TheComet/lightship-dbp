#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/plugin.h>

#define PLUGIN_FREE_INFO_STRING(plugin, strname) \
    if((plugin)->info.strname) \
        free((plugin)->info.strname); \
    (plugin)->info.strname = NULL;

#define PLUGIN_ADD_INFO_STRING(plugin, strname, str) \
    if(str) \
    { \
        (plugin)->info.strname = (char*)malloc((strlen(str)+1) * sizeof(char*)); \
        strcpy((plugin)->info.strname, str); \
    }

struct plugin_t* plugin_create(void)
{
    struct plugin_t* plugin = (struct plugin_t*)malloc(sizeof(struct plugin_t));
    plugin_init_plugin(plugin);
    return plugin;
}

void plugin_init_plugin(struct plugin_t* plugin)
{
    memset(plugin, 0, sizeof(struct plugin_t));
    plugin->info.language = PLUGIN_PROGRAMMING_LANGUAGE_UNSET;
}

void plugin_destroy(struct plugin_t* plugin)
{
    plugin_free_info(plugin);
    free(plugin);
}

void plugin_set_info(struct plugin_t* plugin,
                     const char* name,
                     const char* category,
                     const char* author,
                     const char* description,
                     const char* website)
{
    plugin_free_info(plugin);
    
    PLUGIN_ADD_INFO_STRING(plugin, name, name)
    PLUGIN_ADD_INFO_STRING(plugin, category, category);
    PLUGIN_ADD_INFO_STRING(plugin, author, author)
    PLUGIN_ADD_INFO_STRING(plugin, description, description)
    PLUGIN_ADD_INFO_STRING(plugin, website, website)
}

static void plugin_free_info(struct plugin_t* plugin)
{
    PLUGIN_FREE_INFO_STRING(plugin, name)
    PLUGIN_FREE_INFO_STRING(plugin, category);
    PLUGIN_FREE_INFO_STRING(plugin, author)
    PLUGIN_FREE_INFO_STRING(plugin, description)
    PLUGIN_FREE_INFO_STRING(plugin, website)
}

void plugin_set_programming_language(struct plugin_t* plugin, plugin_programming_language_t language)
{
    plugin->info.language = language;
}

void plugin_set_version(struct plugin_t* plugin, uint32_t major, uint32_t minor, uint32_t patch)
{
    plugin->info.version.major = major;
    plugin->info.version.minor = minor;
    plugin->info.version.patch = patch;
}

void plugin_add_dependency(struct plugin_info_t* plugin)
{
    /* TODO plugin dependencies */
}
    
int plugin_extract_version_from_string(const char* file,
                                       uint32_t* major,
                                       uint32_t* minor,
                                       uint32_t* patch)
{
    /* strtok modifies the character array, copy into temporary */
    char* buffer = (char*)malloc((strlen(file)+1)*sizeof(char*));
    char* temp = buffer;
    char* pch;
    strcpy(buffer, file);
    pch = strtok(temp, "-");
    
    /* extract major, minor, and patch from file name */
    *major = -1;
    *minor = -1;
    *patch = -1;

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

    /* free temporary buffer */
    free(buffer);
    
    /* error check */
    if(*major == -1 || *minor == -1 || *patch == -1)
        return 0;

    return 1;
}

void plugin_get_version_string(char* str, struct plugin_info_t* info)
{
    sprintf(str, "%d-%d-%d",
        info->version.major,
        info->version.minor,
        info->version.patch);
}
