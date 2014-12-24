#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "util/log.h"
#include "util/memory.h"
#include "util/events.h"

#ifdef LOG_ENABLE_TIMESTAMPS
#   include <time.h>
#endif

__inline static int safe_strlen(const char* str)
{
    if(str)
        return strlen(str);
    return 0;
}

__inline static void safe_strcat(char* target, const char* source)
{
    if(source)
        strcat(target, source);
}

void log_init(void)
{
    event_register_listener(NULL, "log", on_llog);
}

LIGHTSHIP_PUBLIC_API void llog(log_level_t level, uint32_t num_strs, ...)
{
#ifdef LOG_ENABLE_TIMESTAMPS
    time_t rawtime;
    struct tm* timeinfo;
    char timestamp[12];
#endif

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
#ifdef LOG_ENABLE_TIMESTAMPS
    rawtime = time(NULL); /* get system time */
    timeinfo = localtime(&rawtime); /* convert to local time */
    total_length = strftime(timestamp, 12, "[%X] ", timeinfo) - 1;
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
    
    /*
     * Get total length of all strings combined and allocate a buffer large
     * enough to hold them, including a null terminator.
     */
    va_start(ap, num_strs);
    for(i = 0; i != num_strs; ++i)
        total_length += safe_strlen(va_arg(ap, char*));
    total_length += strlen(prefix);
    total_length += 2; /* null terminator and newline*/
    va_end(ap);
    
    /* allocate buffer and copy all strings into it */
    buffer = (char*)MALLOC(total_length);
    
    /* copy prefix into buffer */
#ifdef LOG_ENABLE_TIMESTAMPS
    strcpy(buffer, timestamp);
    strcat(buffer, prefix);
#else
    strcpy(buffer, prefix);
#endif
    
    /* copy all other strings into buffer and end with newline*/
    va_start(ap, num_strs);
    for(i = 0; i != num_strs; ++i)
        safe_strcat(buffer, va_arg(ap, char*));
    va_end(ap);
    strcat(buffer, "\n");
    
    /* fire event and clean up */
    log_.level = level;
    log_.message = buffer;
    EVENT_FIRE(evt_log, (void*)(&log_));
    FREE(buffer);
}

EVENT_LISTENER(on_llog)
{
    FILE* fp;
    
    /* determine output */
    switch(((struct log_t*)arg)->level)
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
    
    fprintf(fp, ((struct log_t*)arg)->message);
}