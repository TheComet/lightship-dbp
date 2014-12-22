#include <stdio.h>
#include <dlfcn.h>
#include "util/module_loader.h"
#include "util/string.h"

void* module_open(const char* filename)
{
    void* handle = dlopen(filename, RTLD_LAZY);
    if(!handle)
    {
        stderr_strings(2, "Error loading plugin: ", dlerror());
        return NULL;
    }
    return handle;
}

void* module_sym(void* handle, const char* symbol)
{
    void* ptr;

    dlerror(); /* clear existing errors, if any */
    ptr = dlsym(handle, symbol);
    if(!ptr)
    {
        const char* error = dlerror();
        if(error)
        {
            stderr_strings(2, "Error loading plugin: ", error);
            return NULL;
        }
    }
    return ptr;
}

void module_close(void* handle)
{
    dlclose(handle);
}
