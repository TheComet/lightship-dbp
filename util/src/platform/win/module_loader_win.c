#include <stdio.h>
#include <windows.h>
#include "util/module_loader.h"
#include "util/string.h"
#include "util/platform/win/error.h"
#include "util/memory.h"

void*
module_open(const char* filename)
{
    HINSTANCE handle = LoadLibrary(filename);
    if(handle == NULL)
    {
        char* error = get_last_error_string();
        if(error)
        {
            stderr_strings(2, "Error loading plugin: ", error);
            FREE(error);
        }
    }
    return handle;
}

void*
module_sym(void* handle, const char* symbol)
{
    FARPROC ptr;

    ptr = GetProcAddress((HINSTANCE)handle, symbol);
    if(ptr == NULL)
    {
        char* error = get_last_error_string();
        if(error)
        {
            stderr_strings(2, "Error loading plugin: ", error);
            FREE(error);
        }
    }
    return (void*)ptr;
}

void
module_close(void* handle)
{
    FreeLibrary((HINSTANCE)handle);
}
