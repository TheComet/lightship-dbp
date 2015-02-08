#ifndef LIGHTSHIP_UTIL_PLUGIN_API_H
#define LIGHTSHIP_UTIL_PLUGIN_API_H

#include "util/pstdint.h"

/* these must be implemented by the plugin */
struct game_t;
typedef struct plugin_t*    (*plugin_init_func)     (struct game_t* game);
typedef char                (*plugin_start_func)    (struct game_t* game);
typedef void                (*plugin_stop_func)     (struct game_t* game);
typedef void                (*plugin_deinit_func)   (void);

#define PLUGIN_INIT()  LIGHTSHIP_PUBLIC_API struct plugin_t* plugin_init(struct game_t* game)
#define PLUGIN_START() LIGHTSHIP_PUBLIC_API char plugin_start(struct game_t* game)
#define PLUGIN_STOP()  LIGHTSHIP_PUBLIC_API void plugin_stop(struct game_t* game)
#define PLUGIN_DEINIT()LIGHTSHIP_PUBLIC_API void plugin_deinit(void)

typedef enum plugin_result_t
{
    PLUGIN_FAILURE = 0,
    PLUGIN_SUCCESS = 1
} plugin_result_t;

typedef enum plugin_search_criteria_t
{
    PLUGIN_VERSION_MINIMUM = 0,
    PLUGIN_VERSION_EXACT = 1
} plugin_search_criteria_t;

/*!
 * @brief Programming language the plugin was written in.
 */
typedef enum plugin_programming_language_t
{
    PLUGIN_PROGRAMMING_LANGUAGE_UNSET,
    PLUGIN_PROGRAMMING_LANGUAGE_C,
    PLUGIN_PROGRAMMING_LANGUAGE_CPP,
    PLUGIN_PROGRAMMING_LANGUAGE_D
} plugin_programming_language_t;

/*!
 * @brief API version information of the plugin.
 */
struct plugin_api_version_t
{
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
};

/*!
 * @brief Information about the plugin.
 */
struct plugin_info_t
{
    char* name;
    char* category;
    char* author;
    char* description;
    char* website;
    plugin_programming_language_t language;
    struct plugin_api_version_t version;
};

/*!
 * @brief Plugin object. For every loaded plugin there exists one instance of this.
 */
struct plugin_t
{
    struct plugin_info_t info;
    struct game_t* game;
    void* handle;
    char started_successfully;
    plugin_init_func init;
    plugin_start_func start;
    plugin_stop_func stop;
    plugin_deinit_func deinit;
};

#endif /* LIGHTSHIP_UTIL_PLUGIN_API_H */

