#include <stdio.h>
#include <util\module_loader.h>
#include <util\string.h>
#include <util\platform\win\error.h>
#include <Windows.h>

void* module_open(const char* filename)
{
    HINSTANCE handle = LoadLibrary(filename);
    if(handle == NULL)
    {
        char* error = get_last_error_string();
        if(error)
        {
            fprintf_strings(stderr, 2, "Error loading plugin: ", error);
            free(error);
        }
    }
    return handle;
}

void* module_sym(void* handle, const char* symbol)
{
    FARPROC ptr;

    ptr = GetProcAddress((HINSTANCE)handle, symbol);
    if(ptr == NULL)
    {
        char* error = get_last_error_string();
        if(error)
        {
            fprintf_strings(stderr, 2, "Error loading plugin: ", error);
            free(error);
        }
    }
    return (void*)ptr;
}

void module_close(void* handle)
{
    FreeLibrary((HINSTANCE)handle);
}
