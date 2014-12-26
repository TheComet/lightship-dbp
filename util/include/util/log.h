#ifndef LIGHTSHIP_UTIL_LOG_H
#define LIGHTSHIP_UTIL_LOG_H

#include "util/config.h"
#include "util/pstdint.h"
#include "util/event_api.h"

typedef enum log_level_t
{
    LOG_INFO = 0,
    LOG_WARNING = 1, 
    LOG_ERROR = 2,
    LOG_FATAL = 3,
    LOG_USER = 4
} log_level_t;

struct log_t
{
    log_level_t level;
    char* message;
};

LIGHTSHIP_PUBLIC_API void log_init(void);
LIGHTSHIP_PUBLIC_API void llog_indent(const char* indent_name);
LIGHTSHIP_PUBLIC_API void llog_unindent(void);
LIGHTSHIP_PUBLIC_API void llog(log_level_t level, uint32_t num_strs, ...);

EVENT_LISTENER1(on_llog_indent, const char* str);
EVENT_LISTENER1(on_llog, struct log_t* arg);

#endif /* LIGHTSHIP_UTIL_LOG_H */
