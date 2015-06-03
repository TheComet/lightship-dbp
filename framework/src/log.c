#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "framework/log.h"
#include "framework/game.h"
#include "framework/events.h"
#include "util/memory.h"

#ifdef ENABLE_LOG_TIMESTAMPS
#   include <time.h>
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
    /* initialise indent level */
    game->log.indent_level = 0;

    return 1;
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

    EVENT_FIRE1(game->event.log_indent, indent_name);
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

    EVENT_FIRE0(game->event.log_unindent);
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
    char* tag = NULL;

    /*
     * Get timestamp string.
     * NOTE also sets the total length to the length of the timestamp string.
     */
#ifdef ENABLE_LOG_TIMESTAMPS
    rawtime = time(NULL); /* get system time */
    timeinfo = localtime(&rawtime); /* convert to local time */
    total_length = strftime(timestamp, 12, "[%X] ", timeinfo);
#endif

    /* determine tag string */
    switch(level)
    {
        case LOG_INFO:
            tag = "[INFO] ";
            break;
        case LOG_WARNING:
            tag = "[WARNING] ";
            break;
        case LOG_ERROR:
            tag = "[ERROR] ";
            break;
        case LOG_FATAL:
            tag = "[FATAL] ";
            break;
        case LOG_USER:
            tag = "[USER] ";
            break;
        default:
            tag = "";
            break;
    }
    total_length += strlen(tag);

    /* add length of game string, plus three characters for [] and space */
    if(game)
        total_length += strlen(game->name) + 3;

    /* add length of plugin tag, if any */
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
    va_end(ap);

    /* null terminator and newline */
    total_length += 2;

    /* allocate buffer and copy all strings into it */
    buffer = (char*)MALLOC(total_length);
    *buffer = '\0'; /* so strcat() works */

    /* copy timestamp into buffer */
#ifdef ENABLE_LOG_TIMESTAMPS
    strcat(buffer, timestamp);
#endif

    /* copy tag */
    strcat(buffer, tag);

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

    /* null terminator and newline */
    buffer[total_length-2] = '\n';
    buffer[total_length-1] = '\0';

    /* fire event and output message */
    if(game)
        EVENT_FIRE2(game->event.log, level, buffer);
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
do_indent(FILE* fp, const struct game_t* game)
{
    int i;
    if(game)
    {
        for(i = 0; i != game->log.indent_level; ++i)
            fprintf(fp, "    ");
    }
}

static void
on_llog(const struct game_t* game, log_level_e level, const char* message)
{
    /* indent */
    switch(level)
    {
        case LOG_INFO:
        case LOG_USER:
            do_indent(stdout, game); break;
        default:
            do_indent(stderr, game); break;
    }

    /* output message with the appropriate colours */
    switch(level)
    {
        default:
        case LOG_INFO:      fprintf(stdout, "%s", message);             break;
        case LOG_WARNING:   fprintf(stderr, KYEL "%s" RESET, message);  break;
        case LOG_ERROR:     fprintf(stderr, KMAG "%s" RESET, message);  break;
        case LOG_FATAL:     fprintf(stderr, KRED "%s" RESET, message);  break;
        case LOG_USER:      fprintf(stdout, KCYN "%s" RESET, message);  break;
    }
}
