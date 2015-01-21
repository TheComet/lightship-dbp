#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include "util/string.h"
#include "util/memory.h"

static int
safe_strlen(const char* str)
{
    if(str)
        return strlen(str);
    return 0;
}

static void
safe_strcat(char* target, const char* source)
{
    if(source)
        strcat(target, source);
}

static void
safe_strcpy(char* target, const char* source)
{
    if(source)
        strcpy(target, source);
}

LIGHTSHIP_PUBLIC_API void
stdout_strings(uint32_t num_strs, ...)
{
    uint32_t total_length = 0;
    uint32_t i;
    char* buffer;
    /* compute total length of all strings combined and allocate a buffer able
     * to contain all strings plus a null terminator */
    va_list ap;
    va_start(ap, num_strs);
    for(i = 0; i != num_strs; ++i)
        total_length += safe_strlen(va_arg(ap, char*));
    buffer = (char*)MALLOC((total_length+1) * sizeof(char*));
    va_end(ap);
    
    /* concatenate all strings into the allocated buffer */
    va_start(ap, num_strs);
    safe_strcpy(buffer, va_arg(ap, char*));
    for(i = 1; i < num_strs; ++i)
        safe_strcat(buffer, va_arg(ap, char*));
    va_end(ap);

    /* print to stdout and clean up */
    fprintf(stdout, "%s\n", buffer);
    FREE(buffer);
}

LIGHTSHIP_PUBLIC_API void
stderr_strings(uint32_t num_strs, ...)
{
    uint32_t total_length = 0;
    uint32_t i;
    char* buffer;
    /* compute total length of all strings combined and allocate a buffer able
     * to contain all strings plus a null terminator */
    va_list ap;
    va_start(ap, num_strs);
    for(i = 0; i != num_strs; ++i)
        total_length += safe_strlen(va_arg(ap, char*));
    buffer = (char*)MALLOC((total_length+1) * sizeof(char*));
    va_end(ap);
    
    /* concatenate all strings into the allocated buffer */
    va_start(ap, num_strs);
    safe_strcpy(buffer, va_arg(ap, char*));
    for(i = 1; i < num_strs; ++i)
        safe_strcat(buffer, va_arg(ap, char*));
    va_end(ap);

    /* print to stderr and clean up */
    fprintf(stderr, "%s\n", buffer);
    FREE(buffer);
}

char*
cat_strings(uint32_t num_strs, ...)
{
    uint32_t total_length = 0;
    uint32_t i;
    char* buffer;

    /* compute total length of all strings combined and allocate a buffer able
     * to contain all strings plus a null terminator */
    va_list ap;
    va_start(ap, num_strs);
    for(i = 0; i != num_strs; ++i)
        total_length += safe_strlen(va_arg(ap, char*));
    buffer = (char*)MALLOC((total_length+1) * sizeof(char*));
    va_end(ap);
    
    /* concatenate all strings into the allocated buffer */
    va_start(ap, num_strs);
    safe_strcpy(buffer, va_arg(ap, char*));
    for(i = 1; i < num_strs; ++i)
        safe_strcat(buffer, va_arg(ap, char*));
    va_end(ap);
    
    return buffer;
}

char*
malloc_string(const char* str)
{
    char* buffer = (char*)MALLOC((strlen(str)+1) * sizeof(char*));
    strcpy(buffer, str);
    return buffer;
}

char
is_number(const char chr)
{
    char* numbers = "0123456789";
    while(*numbers)
        if(chr == *numbers++)
            return 1;
    return 0;
}

wchar_t*
strtowcs(const char* str)
{
    wchar_t* wcs;
    wchar_t* wcs_it;
    intptr_t len = strlen(str);
    wcs = (wchar_t*)MALLOC((len + 1) * sizeof(wchar_t));
    for(wcs_it = wcs; *str; ++str)
        *wcs_it++ = (wchar_t)*str;
    *wcs_it = L'\0';
    return wcs;
}

char*
wcstostr(wchar_t* wcs)
{
    char* str;
    char* str_it;
    intptr_t len = wcslen(wcs);
    str = (char*)MALLOC((len + 1) * sizeof(char));
    for(str_it = str; *wcs; ++wcs)
        *str_it++ = (char)*wcs;
    *str_it = '\0';
    return str;
}
