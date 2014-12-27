#include "util/memory.h"
#include "util/unordered_vector.h"
#include "util/backtrace.h"
#include <stdlib.h>
#include <stdio.h>

#define BACKTRACE_OMIT_COUNT 2

#ifdef MEMORY_ENABLE_MEMORY_REPORT
static intptr_t allocations = 0;
static intptr_t deallocations = 0;
static intptr_t ignore_unordered_vector_malloc = 0;
static struct unordered_vector_t report;

struct report_info_t
{
    intptr_t location;
    intptr_t size;
#ifdef MEMORY_ENABLE_BACKTRACE
    intptr_t backtrace_size;
    char** backtrace;
#endif
};

void memory_init(void)
{
    allocations = 0;
    deallocations = 0;
    ignore_unordered_vector_malloc = 0;
    unordered_vector_init_vector(&report, sizeof(struct report_info_t));
}

void* malloc_debug(intptr_t size)
{
    void* p = malloc(size);
    if(p)
        ++allocations;
    
    /* 
     * Record allocation info in vector. Call to vector may allocate memory,
     * so set flag to ignore the call to malloc() when inserting.
     */
    if(!ignore_unordered_vector_malloc)
    {
        struct report_info_t info;
        info.location = (intptr_t)p;
        info.size = size;
#ifdef MEMORY_ENABLE_BACKTRACE
        info.backtrace = get_backtrace(&info.backtrace_size);
#endif
        ignore_unordered_vector_malloc = 1;
        unordered_vector_push(&report, &info);
        ignore_unordered_vector_malloc = 0;
    }

    return p;
}

void free_debug(void* ptr)
{
    free(ptr);
    if(ptr)
        ++deallocations;
    else
        fprintf(stderr, "Warning: free(NULL)");
    
    /* remove the memory location from the vector */
    if(!ignore_unordered_vector_malloc)
    {
        UNORDERED_VECTOR_FOR_EACH(&report, struct report_info_t, info)
        {
            if(info->location == (intptr_t)ptr)
            {
#ifdef MEMORY_ENABLE_BACKTRACE
                free(info->backtrace);
#endif
                unordered_vector_erase_element(&report, info);
                break;
            }
        }
    }
}

void memory_deinit(void)
{
    intptr_t i;
    --allocations; /* this is the single allocation still held by the report vector */
    printf("=========================================\n");
    printf("Memory Report\n");
    printf("=========================================\n");
    if(unordered_vector_count(&report) != 0)
    {
        {
            UNORDERED_VECTOR_FOR_EACH(&report, struct report_info_t, info)
            {
                printf("  un-freed memory at 0x%lx, size 0x%lx\n", info->location, info->size);
#ifdef MEMORY_ENABLE_BACKTRACE
                printf("  Backtrace to where malloc() was called:\n");
                for(i = BACKTRACE_OMIT_COUNT; i < info->backtrace_size; ++i)
                    printf("      %s\n", info->backtrace[i]);
                free(info->backtrace);
                printf("  -----------------------------------------\n");
#endif
            }
        }
        printf("=========================================\n");
    }
    printf("allocations: %lu\n", allocations);
    printf("deallocations: %lu\n", deallocations);
    printf("memory leaks: %lu\n", (allocations > deallocations ? allocations - deallocations : deallocations - allocations));
    printf("=========================================\n");
    ++allocations; /* this is the single allocation still held by the report vector */
    unordered_vector_clear_free(&report);
}
#else /* MEMORY_ENABLE_MEMORY_REPORT */

void memory_init(void) {}
void memory_deinit(void) {}

#endif /* MEMORY_ENABLE_MEMORY_REPORT */
