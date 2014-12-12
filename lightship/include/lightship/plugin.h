#include <util/pstdint.h>

/* these must be implemented by the plugin */
struct plugin_t;
struct services_t;
typedef void (*plugin_start_func)(struct plugin_t*);
typedef void (*plugin_stop_func)(void);

/* programming language the plugin was written in */
typedef enum plugin_programming_language_t
{
    PLUGIN_PROGRAMMING_LANGUAGE_C,
    PLUGIN_PROGRAMMING_LANGUAGE_CPP
} plugin_programming_language_t;

/* api version information of the plugin */
typedef struct plugin_api_version_t
{
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
} plugin_api_version_t;

typedef struct plugin_info_t
{
    char* name;
    char* author;
    char* description;
    char* website;
    plugin_programming_language_t language;
    plugin_api_version_t version;
} plugin_info_t;

typedef struct plugin_t
{
    plugin_info_t info;
    void* handle;
    plugin_start_func start;
    plugin_stop_func stop;
} plugin_t;

plugin_t* plugin_create();
void plugin_init_plugin(plugin_t* plugin);
void plugin_destroy(plugin_t* plugin);
void plugin_set_name(plugin_t* plugin, const char* name);
void plugin_set_programming_language(plugin_t* plugin, plugin_programming_language_t language);
void plugin_set_version(plugin_t* plugin, uint32_t major, uint32_t minior, uint32_t patch);

/* TODO plugin dependencies */
void plugin_add_dependency(plugin_info_t* plugin);
