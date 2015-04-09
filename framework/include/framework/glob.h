#include "util/pstdint.h"

extern uint32_t framework_global_hash;

struct event_t;
struct game_t;

struct framework_glob_events_t
{
    struct event_t* log;
    struct event_t* log_indent;
    struct event_t* log_unindent;
};

struct framework_glob_log_t
{
    char indent_level;
};

struct framework_glob_t
{
    struct game_t* game;
    struct framework_glob_events_t event;
    struct framework_glob_log_t log;
};

/*!
 * @brief Creates and registers an instance a global framework struct with the
 * specified game object.
 * @param[in] game The game to register it to.
 */
void
framework_glob_create(struct game_t* game);

/*!
 * @brief Destroys a registered instance of a global framework struct from the
 * specified game object.
 * @param[in] game The game to unregister from.
 */
void
framework_glob_destroy(struct game_t* game);

#define framework_get_global(game) \
    ((struct framework_glob_t*)game_get_global(game, framework_global_hash))
