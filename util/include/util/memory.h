#include "util/pstdint.h"
#include "util/config.h"

#ifdef ENABLE_MEMORY_REPORT
#   define MALLOC malloc_debug
#   define FREE free_debug
#else
#   define MALLOC malloc
#   define FREE free
#endif

void memory_init(void);
void memory_deinit(void);

#ifdef ENABLE_MEMORY_REPORT
LIGHTSHIP_PUBLIC_API void* malloc_debug(intptr_t size);
LIGHTSHIP_PUBLIC_API void free_debug(void* ptr);
#endif
