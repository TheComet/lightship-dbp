#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "framework/log.h"
#include "framework/game.h"
#include "util/memory.h"

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

static void
on_llog_indent(const struct game_t* game, const char* str);

static void
on_llog_unindent(const struct game_t* game);

static void
on_llog(const struct game_t* game, log_level_e level, const char* message);

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
char
llog_init(struct game_t* game)
{
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
    
    /* initialise indent level */
    game->log.indent_level = 0;
    
    return 1;
}

/* ------------------------------------------------------------------------- */
void
llog_deinit(struct game_t* game)
{
}

/* ------------------------------------------------------------------------- */
void
llog_indent(struct game_t* game, const char* indent_name)
{
    /* can't indent if game is NULL, because the indent level is stored per
     * game */
    if(!game)
    {
        llog(LOG_WARNING, NULL, NULL, 1, "llog_indent() was called with a NULL game object");
        return;
    }

    EVENT_FIRE_FROM_TEMP1(evt_log_indent, game->event.log_indent, indent_name);
    on_llog_indent(game, indent_name);

    ++game->log.indent_level;
}

/* ------------------------------------------------------------------------- */
void
llog_unindent(struct game_t* game)
{
    /* can't unindent if game is NULL, because the indent level is stored per
     * game object */
    if(!game)
    {
        llog(LOG_WARNING, NULL, NULL, 1, "llog_unindent() was called with a NULL game object");
        return;
    }
    
    EVENT_FIRE_FROM_TEMP0(evt_log_unindent, game->event.log_unindent);
    on_llog_unindent(game);

    if(game->log.indent_level)
        --game->log.indent_level;
}

/* ------------------------------------------------------------------------- */
void
llog(log_level_e level, const struct game_t* game, const char* plugin, uint32_t num_strs, ...)
{
    /* variables required to generate a timestamp string */
#ifdef ENABLE_LOG_TIMESTAMPS
    time_t rawtime;
    struct tm* timeinfo;
    char timestamp[12];
#endif

    /* more local variables because C89 */
    va_list ap;
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
    
    /* add length of game string, plus three characters for [] and space */
    if(game)
        total_length += strlen(game->name) + 3;

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
    *buffer = '\0'; /* so strcat() works */
    
    /* copy timestamp into buffer */
#ifdef ENABLE_LOG_TIMESTAMPS
    strcat(buffer, timestamp);
#endif
    
    /* copy game name */
    if(game)
    {
        strcat(buffer, "[");
        strcat(buffer, game->name);
        strcat(buffer, "] ");
    }
    
    /* copy plugin name into buffer, if any */
    if(plugin)
    {
        strcat(buffer, "[");
        strcat(buffer, plugin);
        strcat(buffer, "] ");
    }
    
    /* copy all other strings into buffer and end with newline */
    va_start(ap, num_strs);
    for(i = 0; i != num_strs; ++i)
        safe_strcat(buffer, va_arg(ap, char*));
    va_end(ap);
    /*strcat(buffer, "\n");*/
    buffer[total_length-2] = '\n';
    buffer[total_length-1] = '\0';

    /* fire event and output mesasge */
    if(game)
        EVENT_FIRE_FROM_TEMP2(evt_log, game->event.log, level, (const char*)buffer);
    on_llog(game, level, buffer);

    FREE(buffer);
}

/* ------------------------------------------------------------------------- */
void
llog_critical_use_no_memory(const char* message)
{
    printf("[FATAL] %s\n", message);
}

/* ----------------------------------------------------------------------------
 * Built in event listeners
 * ------------------------------------------------------------------------- */
static void
on_llog_indent(const struct game_t* game, const char* str)
{
    llog(LOG_INFO, game, NULL, 1, str);
}

/* ------------------------------------------------------------------------- */
static void
on_llog_unindent(const struct game_t* game)
{
}

/* ------------------------------------------------------------------------- */
static void
on_llog(const struct game_t* game, log_level_e level, const char* message)
{
    FILE* fp;
    char i;
    
    /* determine output stream for console output */
    switch(level)
    {
        case LOG_INFO:
        case LOG_USER:
            fp = stdout;
            break;
        default:
            fp = stderr;
            break;
    }

    /* get global data for indentation if possible */
    if(game)
    {
        for(i = 0; i != game->log.indent_level; ++i)
            fprintf(fp, "    ");
    }
    
    fprintf(fp, "%s\n", message);
}
