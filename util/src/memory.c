#include "util/memory.h"
#include "util/map.h"
#include "util/backtrace.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BACKTRACE_OMIT_COUNT 2

#ifdef MEMORY_ENABLE_MEMORY_REPORT
static intptr_t allocations = 0;
static intptr_t deallocations = 0;
static intptr_t ignore_map_malloc = 0;
static struct map_t report;

struct report_info_t
{
    intptr_t location;
    intptr_t size;
#ifdef MEMORY_ENABLE_BACKTRACE
    intptr_t backtrace_size;
    char** backtrace;
#endif
};

void
memory_init(void)
{
    allocations = 0;
    deallocations = 0;
    ignore_map_malloc = 0;
    map_init_map(&report);
}

void*
malloc_debug(intptr_t size)
{
    void* p = malloc(size);
    if(p)
        ++allocations;
    
    /* 
     * Record allocation info in vector. Call to vector may allocate memory,
     * so set flag to ignore the call to malloc() when inserting.
     */
    if(!ignore_map_malloc)
    {
        struct report_info_t* info = (struct report_info_t*)malloc(sizeof(struct report_info_t));;
        info->location = (intptr_t)p;
        info->size = size;
#ifdef MEMORY_ENABLE_BACKTRACE
        info->backtrace = get_backtrace(&info->backtrace_size);
#endif
        ignore_map_malloc = 1;
        map_insert(&report, info->location, info);
        ignore_map_malloc = 0;
    }

    return p;
}

void
free_debug(void* ptr)
{
    free(ptr);
    if(ptr)
        ++deallocations;
    else
        fprintf(stderr, "Warning: free(NULL)\n");
    
    /* remove the memory location from the vector */
    if(!ignore_map_malloc)
    {
        struct report_info_t* info = (struct report_info_t*)map_find(&report, (intptr_t)ptr);
        if(info)
        {
#ifdef MEMORY_ENABLE_BACKTRACE
            free(info->backtrace);
#endif
            map_erase(&report, (intptr_t)ptr);
            free(info);
        }else
            printf("HORRIBLE HORRIBLE ERROR, WHAT THE FUCK THIS SHOULD NEVER HAPPEN\n");
    }
}

void
memory_deinit(void)
{
#ifdef MEMORY_ENABLE_BACKTRACE
    intptr_t i;
#endif
    --allocations; /* this is the single allocation still held by the report vector */
    printf("=========================================\n");
    printf("Memory Report\n");
    printf("=========================================\n");
    if(map_count(&report) != 0)
    {
        {
            MAP_FOR_EACH(&report, struct report_info_t, info)
            {
                char* dump;
                printf("  un-freed memory at 0x%lx, size 0x%lx\n", info->location, info->size);
                
                /* print a string dump of the unfreed data */
                dump = malloc(info->size + 1);
                memcpy(dump, (void*)info->location, info->size);
                dump[info->size] = '\0';
                printf("  string dump: %s\n", dump);
                free(dump);

#ifdef MEMORY_ENABLE_BACKTRACE
                printf("  Backtrace to where malloc() was called:\n");
                for(i = BACKTRACE_OMIT_COUNT; i < info->backtrace_size; ++i)
                    printf("      %s\n", info->backtrace[i]);
                free(info->backtrace);
                printf("  -----------------------------------------\n");
#endif
                free(info);
            }
        }
        printf("=========================================\n");
    }
    printf("allocations: %lu\n", allocations);
    printf("deallocations: %lu\n", deallocations);
    printf("memory leaks: %lu\n", (allocations > deallocations ? allocations - deallocations : deallocations - allocations));
    printf("=========================================\n");
    ++allocations; /* this is the single allocation still held by the report vector */
    map_clear(&report);
}
#else /* MEMORY_ENABLE_MEMORY_REPORT */

void memory_init(void) {}
void memory_deinit(void) {}

#endif /* MEMORY_ENABLE_MEMORY_REPORT */
