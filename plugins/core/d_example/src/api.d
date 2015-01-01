struct lightship_api_t;

extern(C) alias plugin_init_func = plugin_t function(lightship_api_t*);
extern(C) alias plugin_start_func = char function(lightship_api_t*);
extern(C) alias plugin_stop_func = void function();

enum plugin_programming_language_t
{
    PLUGIN_PROGRAMMING_LANGUAGE_UNSET,
    PLUGIN_PROGRAMMING_LANGUAGE_C,
    PLUGIN_PROGRAMMING_LANGUAGE_CPP,
    PLUGIN_PROGRAMMING_LANGUAGE_D
}

struct plugin_api_version_t
{
    uint major;
    uint minor;
    uint patch;
}

struct plugin_info_t
{
    char* name;
    char* category;
    char* author;
    char* description;
    char* website;
    plugin_programming_language_t language;
    plugin_api_version_t v;
}

struct plugin_t
{
    plugin_info_t info;
    void* handle;
    plugin_init_func init;
    plugin_start_func start;
    plugin_stop_func stop;
}

extern (C) {
  extern plugin_t* plugin_create();
  extern void plugin_set_info(plugin_t* plugin, immutable(char)* name, immutable(char)* category, immutable(char)* author, immutable(char)* description, immutable(char)* website);
  extern void plugin_set_version(plugin_t* plugin, uint major, uint minior, uint patch);
  extern void plugin_destroy(plugin_t* plugin);
}

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
