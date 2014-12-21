#include "util/memory.h"
#include <stdlib.h>
#include <stdio.h>

static unsigned long allocations = 0;
static unsigned long deallocations = 0;

void* malloc_debug(intptr_t size)
{
    ++allocations;
    return malloc(size);
}

void free_debug(void* ptr)
{
    ++deallocations;
    free(ptr);
}

void memory_report(void)
{
#ifdef _DEBUG
    printf("Memory Report:\n");
    printf("  allocations: %lu\n", allocations);
    printf("  deallocations: %lu\n", deallocations);
    printf("  memory leaks: %lu\n", (allocations > deallocations ? allocations - deallocations : deallocations - allocations));
#endif
}
