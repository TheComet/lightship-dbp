#include <util/config.h>

#ifdef LIGHTSHIP_PLATFORM_LINUX

#include <stdio.h>
#include <util/string.h>
#include <dlfcn.h>

void* module_open(const char* filename)
{
    void* handle = dlopen(filename, RTLD_LAZY);
    if(!handle)
    {
        fprintf_strings(stderr, 2, "Error loading plugin: ", dlerror());
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
            fprintf_strings(stderr, 2, "Error loading plugin: ", error);
            return NULL;
        }
    }
    return ptr;
}

void module_close(void* handle)
{
    dlclose(handle);
}

#endif /* #ifdef LIGHTSHIP_PLATFORM_LINUX */