#include <util/pstdint.h>

/* these must be implemented by the plugin */
struct plugin_t;
typedef struct plugin_t* (*plugin_start_func)(void);
typedef void (*plugin_stop_func)(void);

/* programming language the plugin was written in */
typedef enum plugin_programming_language_e
{
    plugin_programming_language_c,
    plugin_programming_language_cpp
} plugin_programming_language_e;

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
    plugin_programming_language_e language;
    plugin_api_version_t version;
} plugin_info_t;

typedef struct plugin_t
{
    plugin_info_t info;
    void* handle;
    plugin_start_func start;
    plugin_stop_func stop;
} plugin_t;
