#include "util/memory.h"
#include "util/vector.h"
#include <stdlib.h>
#include <stdio.h>

static intptr_t allocations = 0;
static intptr_t deallocations = 0;
static intptr_t ignore_vector_malloc = 0;
static struct vector_t report;

struct report_info_t
{
    intptr_t location;
    intptr_t size;
};

void memory_init(void)
{
#ifdef _DEBUG
    allocations = 0;
    deallocations = 0;
    ignore_vector_malloc = 0;
    vector_init_vector(&report, sizeof(struct report_info_t));
#endif
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
    if(!ignore_vector_malloc)
    {
        struct report_info_t info;
        info.location = (intptr_t)p;
        info.size = size;
        ignore_vector_malloc = 1;
        vector_push(&report, &info);
        ignore_vector_malloc = 0;
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
    
    if(!ignore_vector_malloc)
    {
        VECTOR_FOR_EACH(&report, struct report_info_t, info)
        {
            if(info->location == (intptr_t)ptr)
            {
                vector_erase_index(&report, ( ((DATA_POINTER_TYPE*)info) - report.data) / report.element_size);
                break;
            }
        }
    }
}

void memory_deinit(void)
{
#ifdef _DEBUG
    --allocations; /* this is the single allocation still held by the report vector */
    printf("Memory Report:\n");
    printf("  allocations: %lu\n", allocations);
    printf("  deallocations: %lu\n", deallocations);
    printf("  memory leaks: %lu\n", (allocations > deallocations ? allocations - deallocations : deallocations - allocations));
    if(vector_count(&report) != 0)
    {
        printf("  -----------------------------------------\n");
        {
            VECTOR_FOR_EACH(&report, struct report_info_t, info)
            {
                printf("    un-freed memory at 0x%x, size 0x%x\n", info->location, info->size);
            }
        }
        printf("  -----------------------------------------\n");
    }
    vector_clear_free(&report);
#endif
}
