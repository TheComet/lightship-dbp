#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "util/log.h"
#include "util/memory.h"
#include "util/events.h"
#include "util/config.h"

#ifdef ENABLE_LOG_TIMESTAMPS
#   include <time.h>
#endif

#ifdef HAVE_CURSES
    /* include correct header file */
#   ifdef CURSES_HAVE_CURSES_H
#       include "curses.h"
#   elif defined (CURSES_HAVE_NCURSES_H)
#       include "ncurses.h"
#   elif defined (CURSES_HAVE_NCURSES_NCURSES_H)
#       include "ncurses/ncurses.h"
#   elif defined (CURSES_HAVE_NCURSES_CURSES_H)
#       include "ncurses/curses.h"
#   else
#       error Failed to determine which ncurses header file to include. Check config.h.in and util/CMakeLists.txt.
#   endif
    /* remap colours */
#   define COLOR_ORANGE COLOR_YELLOW
#   define COLOR_PURPLE COLOR_MAGENTA
#endif

static char g_log_indent = 0;

/* ----------------------------------------------------------------------------
 * Static functions
 * ------------------------------------------------------------------------- */
static int
safe_strlen(const char* str)
{
    if(str)
        return strlen(str);
    return 0;
}

/* ------------------------------------------------------------------------- */
static void
safe_strcat(char* target, const char* source)
{
    if(source)
        strcat(target, source);
}

/* ----------------------------------------------------------------------------
 * Exported functions
 * ------------------------------------------------------------------------- */
void
llog_init(struct game_t* game)
{
    event_register_listener(game, NULL, BUILTIN_NAMESPACE_NAME ".log", (event_callback_func)on_llog);
    event_register_listener(game, NULL, BUILTIN_NAMESPACE_NAME ".log_indent", (event_callback_func)on_llog_indent);
    
    /* ncurses support -- TODO currently broken and disabled *
#ifdef HAVE_CURSES
    initscr();
    start_color();
    
    if(can_change_color())
    {
        *init_color(COLOR_ORANGE, 1000, 500, 0);
        init_color(COLOR_PURPLE, 0, 500, 1000);*
    }

    init_pair(LOG_WARNING, COLOR_ORANGE, -1);
    init_pair(LOG_ERROR, COLOR_RED, -1);
    init_pair(LOG_FATAL, -1, COLOR_RED);
    init_pair(LOG_USER, COLOR_PURPLE, -1);
#endif  */
}

/* ------------------------------------------------------------------------- */
void
llog_indent(const char* indent_name)
{
    EVENT_FIRE1(evt_log_indent, indent_name);
    ++g_log_indent;
}

/* ------------------------------------------------------------------------- */
void
llog_unindent(void)
{
    EVENT_FIRE0(evt_log_unindent);
    if(g_log_indent)
        --g_log_indent;
}

/* ------------------------------------------------------------------------- */
void
llog(log_level_t level, const char* plugin, uint32_t num_strs, ...)
{
    /* variables required to generate a timestamp string */
#ifdef ENABLE_LOG_TIMESTAMPS
    time_t rawtime;
    struct tm* timeinfo;
    char timestamp[12];
#endif

    /* more local variables because C89 */
    va_list ap;
    struct log_t log_;
    uint32_t i;
    uint32_t total_length = 0;
    char* buffer = NULL;
    char* prefix = NULL;

    /* 
     * Get timestamp string.
     * NOTE also sets the total length to the length of the timestamp string.
     */
#ifdef ENABLE_LOG_TIMESTAMPS
    rawtime = time(NULL); /* get system time */
    timeinfo = localtime(&rawtime); /* convert to local time */
    total_length = strftime(timestamp, 12, "[%X] ", timeinfo);
#endif

    /* determine prefix string */
    switch(level)
    {
        case LOG_INFO:
            prefix = "[INFO] ";
            break;
        case LOG_WARNING:
            prefix = "[WARNING] ";
            break;
        case LOG_ERROR:
            prefix = "[ERROR] ";
            break;
        case LOG_FATAL:
            prefix = "[FATAL] ";
            break;
        case LOG_USER:
            prefix = "[USER] ";
            break;
        default:
            prefix = "";
            break;
    }
    
    /* add length of plugin prefix, if any */
    if(plugin)
    {
        total_length += strlen(plugin) + 3;
        /* +3 for brackes[] and space */
    }

    /*
     * Get total length of all strings combined and allocate a buffer large
     * enough to hold them, including a null terminator.
     */
    va_start(ap, num_strs);
    for(i = 0; i != num_strs; ++i)
        total_length += safe_strlen(va_arg(ap, char*));
    total_length += strlen(prefix);
    total_length += 2; /* null terminator and newline */
    va_end(ap);
    
    /* allocate buffer and copy all strings into it */
    buffer = (char*)MALLOC(total_length);
    
    /* copy prefix into buffer */
#ifdef ENABLE_LOG_TIMESTAMPS
    strcpy(buffer, timestamp);
    strcat(buffer, prefix);
#else
    strcpy(buffer, prefix);
#endif
    
    /* copy plugin prefix into buffer, if any */
    if(plugin)
    {
        strcat(buffer, "[");
        strcat(buffer, plugin);
        strcat(buffer, "] ");
    }
    
    /* copy all other strings into buffer and end with newline*/
    va_start(ap, num_strs);
    for(i = 0; i != num_strs; ++i)
        safe_strcat(buffer, va_arg(ap, char*));
    va_end(ap);
    /*strcat(buffer, "\n");*/
    buffer[total_length-2] = '\n';
    buffer[total_length-1] = '\0';

    /* fire event and clean up */
    log_.level = level;
    log_.message = buffer;
    EVENT_FIRE1(evt_log, &log_);
    FREE(buffer);
}

void
llog_critical_use_no_memory(const char* message)
{
    printf("[FATAL] %s\n", message);
}

/* ----------------------------------------------------------------------------
 * Event listeners
 * ------------------------------------------------------------------------- */
EVENT_LISTENER1(on_llog_indent, const char* str)
{
    llog(LOG_INFO, NULL, 1, str);
}

/* ------------------------------------------------------------------------- */
EVENT_LISTENER1(on_llog, struct log_t* arg)
{
    FILE* fp;
    char i;
    
    /* determine output stream */
    switch(arg->level)
    {
        case LOG_INFO:
            fp = stdout;
            break;
        case LOG_USER:
            fp = stdout;
            break;
        default:
            fp = stderr;
            break;
    }

    for(i = 0; i != g_log_indent; ++i)
        fprintf(fp, "    ");
    fprintf(fp, "%s", arg->message);
}
