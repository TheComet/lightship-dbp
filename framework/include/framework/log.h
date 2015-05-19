#ifndef FRAMEWORK_LOG_H
#define FRAMEWORK_LOG_H

#include "util/pstdint.h"
#include "framework/config.h"
#include "framework/event_api.h"

C_HEADER_BEGIN

/* http://stackoverflow.com/questions/3585846/color-text-in-terminal-aplications-in-unix*/
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\033[0m"

struct log_t;
struct game_t;

typedef enum log_level_e
{
    LOG_INFO = 0,
    LOG_WARNING = 1,
    LOG_ERROR = 2,
    LOG_FATAL = 3,
    LOG_USER = 4,
    LOG_NONE = 5
} log_level_e;

/* built in events */
EVENT_H2(evt_log, log_level_e, const char*)
EVENT_H1(evt_log_indent, const char*)
EVENT_H0(evt_log_unindent)

/*!
 * @brief Initialises the log for the specified game object. Must be called
 * before llog() can be called with a non-NULL game object.
 */
FRAMEWORK_PUBLIC_API char
llog_init(struct game_t* game);

FRAMEWORK_PUBLIC_API void
llog_set_events(struct event_t* on_indent, struct event_t* on_unindent, struct event_t* on_log);

/*!
 * @brief Opens an indentation level of the log.
 *
 * This causes every succeeding call to llog() to be indented by a certain
 * amount, until llog_unindent() is called.
 * @param[in] indent_name The name of the new indentation level.
 */
FRAMEWORK_PUBLIC_API void
llog_indent(struct game_t* game, const char* indent_name);

/*!
 * @brief Closes one indentation level of the log.
 *
 * This causes every succeeding call to llog() to be indented one level less
 * than before. If the indent level is at 0, then nothing happens.
 */
FRAMEWORK_PUBLIC_API void
llog_unindent(struct game_t* game);

/*!
 * @brief Fires a log event with the specified information.
 * @param[in] level The severity of the message being logged. Possible
 * parameters are *LOG_INFO*, *LOG_WARNING*, *LOG_ERROR*, *LOG_FATAL*,
 * *LOG_USER*, and *LOG_NONE*. If the parameter is not *LOG_NONE*, then the
 * message being logged will be prefixed by a respective indicator of severity.
 * @param[in] num_strs The number of strings that are going to be passed to this
 * function. This is required for varargs to know how to concatenate the strings
 * together.
 * @param[in] strs... The strings to concatenate and send to the log.
 */
FRAMEWORK_PUBLIC_API void
llog(log_level_e level, const struct game_t* game, const char* plugin, uint32_t num_strs, ...);

FRAMEWORK_PUBLIC_API void
llog_critical_use_no_memory(const char* message);

C_HEADER_END

#endif /* FRAMEWORK_LOG_H */
