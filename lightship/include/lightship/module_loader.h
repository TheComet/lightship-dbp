#include <util/config.h>

#ifdef LIGHTSHIP_PLATFORM_LINUX
#   include <dlfcn.h>
#   include <dirent.h>
#   include <lightship/module_loader_linux.h>
#elif defined(LIGHTSHIP_PLATFORM_WINDOWS)
#endif

int* module_open(const char* filename);
int* module_sym(void* handle, const char* symbol);
int* module_close(void* handle);