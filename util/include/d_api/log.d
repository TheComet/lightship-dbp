module api.log;

private enum log_level_t
{
    LOG_INFO = 0,
    LOG_WARNING = 1, 
    LOG_ERROR = 2,
    LOG_FATAL = 3,
    LOG_USER = 4,
    LOG_NONE = 5
}

private struct log_t
{
    log_level_t level;
    char* message;
}

private extern (C) {
    extern static void llog_indent(immutable(char)* indent_name);
    extern static void llog_unindent();
    extern static void llog(log_level_t level, uint num_strs, ...);
}

class Log {
    static void log_info(string[] messages...) {
        log(log_level_t.LOG_INFO, messages);
    }

    static void log_warning(string[] messages...) {
        log(log_level_t.LOG_WARNING, messages);
    }

    static void log_error(string[] messages...) {
        log(log_level_t.LOG_ERROR, messages);
    }

    static void log_fatal(string[] messages...) {
        log(log_level_t.LOG_FATAL, messages);
    }

    static void log_user(string[] messages...) {
        log(log_level_t.LOG_USER, messages);
    }

    static void log_none(string[] messages...) {
        log(log_level_t.LOG_NONE, messages);
    }

    private static void log(log_level_t level, string[] messages...) {
        import std.array;

        llog(level, 1, join(messages).ptr);
    }
}
