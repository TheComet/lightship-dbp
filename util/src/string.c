#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/string.h>

inline static int safe_strlen(const char* str)
{
    if(str)
        return strlen(str);
    return 0;
}

inline static void safe_strcat(char* target, const char* source)
{
    if(source)
        strcat(target, source);
}

void fprintf_strings(FILE* file, int num_strs, ...)
{
    /* compute total length of all strings combined and allocate a buffer able
     * to contain all strings plus a null terminator */
    va_list ap;
    va_start(ap, num_strs);
    int total_length = 0;
    for(int i = 0; i != num_strs; ++i)
        total_length += safe_strlen(va_arg(ap, char*));
    total_length += sizeof(char*); /* null terminator */
    char* buffer = malloc(total_length * sizeof(char*));
    va_end(ap);
    
    /* concatinate all strings into the allocated buffer */
    va_start(ap, num_strs);
    strcpy(buffer, va_arg(ap, char*));
    for(int i = 1; i < num_strs; ++i)
        safe_strcat(buffer, va_arg(ap, char*));
    va_end(ap);
    
    /* finally, print to file and clean up */
    fprintf(file, "%s\n", buffer);
    free(buffer);
}

int is_number(const char chr)
{
    char* numbers = "0123456789";
    while(numbers)
        if(chr == *numbers++)
            return 1;
    return 0;
}