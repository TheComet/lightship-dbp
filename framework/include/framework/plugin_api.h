#ifndef FRAMEWORK_PLUGIN_API_H
#define FRAMEWORK_PLUGIN_API_H

#include "util/pstdint.h"
#include "util/unordered_vector.h"

/* these must be implemented by the plugin */
struct game_t;
struct plugin_t;
typedef struct plugin_t*    (*plugin_init_func)     (struct game_t* game);
typedef char                (*plugin_start_func)    (struct game_t* game);
typedef void                (*plugin_stop_func)     (struct game_t* game);
typedef void                (*plugin_deinit_func)   (struct game_t* game);

#define PLUGIN_INIT()   struct plugin_t* plugin_init(struct game_t* game)
#define PLUGIN_START()  char plugin_start(struct game_t* game)
#define PLUGIN_STOP()   void plugin_stop(struct game_t* game)
#define PLUGIN_DEINIT() void plugin_deinit(struct game_t* game)

typedef enum plugin_result_t
{
	PLUGIN_FAILURE = 0,
	PLUGIN_SUCCESS = 1
} plugin_result_t;

typedef enum plugin_search_criteria_e
{
	PLUGIN_VERSION_MINIMUM = 0, /* this must start at 0 because it's used as an index for an array' */
	PLUGIN_VERSION_EXACT = 1
} plugin_search_criteria_e;

/*!
 * @brief Programming language the plugin was written in.
 */
typedef enum plugin_programming_language_e
{
	PLUGIN_PROGRAMMING_LANGUAGE_UNSET,
	PLUGIN_PROGRAMMING_LANGUAGE_C,
	PLUGIN_PROGRAMMING_LANGUAGE_CPP,
	PLUGIN_PROGRAMMING_LANGUAGE_D
} plugin_programming_language_e;

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
	char is_optional;
	plugin_programming_language_e language;
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
	struct unordered_vector_t events;
	struct unordered_vector_t services;
	plugin_init_func init;
	plugin_start_func start;
	plugin_stop_func stop;
	plugin_deinit_func deinit;
};

#endif /* FRAMEWORK_PLUGIN_API_H */
