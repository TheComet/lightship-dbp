#include "framework/log.h"
#include "util/memory.h"
#include "util/string.h"
#include "framework/plugin.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define PLUGIN_FREE_INFO_STRING(plugin, strname)        \
    if((plugin)->info.strname)                          \
        free_string((plugin)->info.strname);            \
    (plugin)->info.strname = NULL;

#define PLUGIN_ADD_INFO_STRING(plugin, strname, str)    \
    if(str)                                             \
    {                                                   \
        (plugin)->info.strname = malloc_string(str);    \
    }

struct service_t;

/* ----------------------------------------------------------------------------
 * Static functions
 * ------------------------------------------------------------------------- */

/*!
 * @brief Copies the info strings into the plugin object, and frees any strings
 * that were there before.
 */
static void
plugin_set_info(struct plugin_t* plugin,
                const char* name,
                const char* category,
                const char* author,
                const char* description,
                const char* website);
/*!
 * @brief Frees all buffers allocated for info strings.
 */
static void
plugin_free_info(struct plugin_t* plugin);

/* ----------------------------------------------------------------------------
 * Exported functions
 * ------------------------------------------------------------------------- */
struct plugin_t*
plugin_create(struct game_t* game,
              const char* name,
              const char* category,
              const char* author,
              const char* description,
              const char* website)
{
    struct plugin_t* plugin = (struct plugin_t*)MALLOC(sizeof(struct plugin_t));
    if(!plugin)
        OUT_OF_MEMORY("plugin_create()", NULL);
    plugin_init_plugin(game, plugin);
    plugin_set_info(plugin, name, category, author, description, website);
    return plugin;
}

/* ------------------------------------------------------------------------- */
void
plugin_init_plugin(struct game_t* game, struct plugin_t* plugin)
{
    memset(plugin, 0, sizeof(struct plugin_t));
    unordered_vector_init_vector(&plugin->events, sizeof(struct event_t*));
    unordered_vector_init_vector(&plugin->services, sizeof(struct service_t*));
    plugin->game = game;
    plugin->info.language = PLUGIN_PROGRAMMING_LANGUAGE_UNSET;
}

/* ------------------------------------------------------------------------- */
void
plugin_destroy(struct plugin_t* plugin)
{
    plugin_free_info(plugin);
    unordered_vector_clear_free(&plugin->services);
    unordered_vector_clear_free(&plugin->events);
    FREE(plugin);
}

/* ------------------------------------------------------------------------- */
static void
plugin_set_info(struct plugin_t* plugin,
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

/* ------------------------------------------------------------------------- */
void
plugin_set_programming_language(struct plugin_t* plugin,
                                plugin_programming_language_t language)
{
    plugin->info.language = language;
}

/* ------------------------------------------------------------------------- */
void
plugin_set_version(struct plugin_t* plugin,
                   uint32_t major,
                   uint32_t minor,
                   uint32_t patch)
{
    plugin->info.version.major = major;
    plugin->info.version.minor = minor;
    plugin->info.version.patch = patch;
}

/* ------------------------------------------------------------------------- */
char
plugin_extract_version_from_string(const char* file,
                                   uint32_t* major,
                                   uint32_t* minor,
                                   uint32_t* patch)
{
    /* strtok modifies the character array, copy into temporary */
    char* buffer = (char*)MALLOC((strlen(file)+1)*sizeof(char));
    char* temp = buffer;
    char* pch;
    const char* delim = ".";
    strcpy(buffer, file);

    /* extract major, minor, and patch from file name */
    *major = -1;
    *minor = -1;
    *patch = -1;

    /* skip ahead until a token is found that contains a number. */
    pch = strtok(temp, delim);
    while(pch != NULL)
    {
        if(strpbrk(pch, "0123456789") != NULL)
            break;
        pch = strtok(NULL, delim);
    }
    /* the following numbers must be major, minor, and patch numbers */
    if(pch != NULL)
        *major = atoi(pch);
    if((pch = strtok(NULL, delim)) != NULL)
        *minor = atoi(pch);
    if((pch = strtok(NULL, delim)) != NULL)
        *patch = atoi(pch);

    /* FREE temporary buffer */
    FREE(buffer);

    /* error check */
    if(*major == (uint32_t)(-1) || *minor == (uint32_t)(-1) || *patch == (uint32_t)(-1))
        return 0;

    return 1;
}

/* ------------------------------------------------------------------------- */
void
plugin_get_version_string(char* str, const struct plugin_info_t* info)
{
    sprintf(str, "%d.%d.%d",
        info->version.major,
        info->version.minor,
        info->version.patch);
}

/* ----------------------------------------------------------------------------
 * Static functions
 * ------------------------------------------------------------------------- */
static void
plugin_free_info(struct plugin_t* plugin)
{
    PLUGIN_FREE_INFO_STRING(plugin, name)
    PLUGIN_FREE_INFO_STRING(plugin, category);
    PLUGIN_FREE_INFO_STRING(plugin, author)
    PLUGIN_FREE_INFO_STRING(plugin, description)
    PLUGIN_FREE_INFO_STRING(plugin, website)
}
