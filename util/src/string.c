#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/string.h>

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

__inline static void safe_strcpy(char* target, const char* source)
{
    if(source)
        strcpy(target, source);
}

void fprintf_strings(FILE* file, int num_strs, ...)
{
    int i;
    char* buffer;
    int total_length = 0;

    /* compute total length of all strings combined and allocate a buffer able
     * to contain all strings plus a null terminator */
    va_list ap;
    va_start(ap, num_strs);
    for(i = 0; i != num_strs; ++i)
        total_length += safe_strlen(va_arg(ap, char*));
    buffer = (char*)malloc((total_length+1) * sizeof(char*));
    va_end(ap);
    
    /* concatenate all strings into the allocated buffer */
    va_start(ap, num_strs);
    safe_strcpy(buffer, va_arg(ap, char*));
    for(i = 1; i < num_strs; ++i)
        safe_strcat(buffer, va_arg(ap, char*));
    va_end(ap);
    
    /* finally, print to file and clean up */
    fprintf(file, "%s\n", buffer);
    free(buffer);
}

char* cat_strings(int num_strs, ...)
{
    int total_length = 0;
    int i;
    char* buffer;
    /* compute total length of all strings combined and allocate a buffer able
     * to contain all strings plus a null terminator */
    va_list ap;
    va_start(ap, num_strs);
    for(i = 0; i != num_strs; ++i)
        total_length += safe_strlen(va_arg(ap, char*));
    buffer = (char*)malloc((total_length+1) * sizeof(char*));
    va_end(ap);
    
    /* concatenate all strings into the allocated buffer */
    va_start(ap, num_strs);
    safe_strcpy(buffer, va_arg(ap, char*));
    for(i = 1; i < num_strs; ++i)
        safe_strcat(buffer, va_arg(ap, char*));
    va_end(ap);
    
    return buffer;
}

char* malloc_string(const char* str)
{
    char* buffer = (char*)malloc((strlen(str)+1) * sizeof(char*));
    strcpy(buffer, str);
    return buffer;
}

int is_number(const char chr)
{
    char* numbers = "0123456789";
    while(*numbers)
        if(chr == *numbers++)
            return 1;
    return 0;
}
