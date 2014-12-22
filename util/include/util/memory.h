#include "util/pstdint.h"

#ifdef _DEBUG
#   define MALLOC malloc_debug
#   define FREE free_debug
#else
#   define MALLOC malloc
#   define FREE free
#endif

void memory_init(void);
void* malloc_debug(intptr_t size);
void free_debug(void* ptr);
void memory_deinit(void);
