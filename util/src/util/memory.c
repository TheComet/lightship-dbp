#include "util/memory.h"
#include "util/map.h"
#include "util/backtrace.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define BACKTRACE_OMIT_COUNT 2

#ifdef ENABLE_MEMORY_REPORT
static uintptr_t allocations = 0;
static uintptr_t deallocations = 0;
static uintptr_t ignore_map_malloc = 0;
static struct map_t report;

#   ifdef ENABLE_MEMORY_EXPLICIT_MALLOC_FAILS
static volatile int malloc_fail_counter = 0;
#   endif /* ENABLE_MEMORY_EXPLICIT_MALLOC_FAILS */

/* need a mutex to make custom_malloc_debug() and free_debug() thread safe */
/* NOTE: Mutex must be recursive */
#   if defined(ENABLE_MULTITHREADING) || defined(ENABLE_MEMORY_EXPLICIT_MALLOC_FAILS)
#       if defined(LIGHTSHIP_UTIL_PLATFORM_LINUX) || defined(LIGHTSHIP_UTIL_PLATFORM_MACOSX)
#           include <pthread.h>
#           define MUTEX pthread_mutex_t
#           define MUTEX_LOCK(x) pthread_mutex_lock(&(x));
#           define MUTEX_UNLOCK(x) pthread_mutex_unlock(&(x));
#           define MUTEX_INIT(x) do {                                           \
                pthread_mutexattr_t attr;                                       \
                pthread_mutexattr_init(&attr);                                  \
                pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);      \
                pthread_mutex_init(&(x), &attr);                                \
                pthread_mutexattr_destroy(&attr);                               \
            } while(0);
#           define MUTEX_DEINIT(x) pthread_mutex_destroy(&(x));

#       else /* defined(LIGHTSHIP_UTIL_PLATFORM_LINUX) || defined(LIGHTSHIP_UTIL_PLATFORM_MACOSX) */
#           error Dont know how to create a mutex for the target platform. Either disable ENABLE_MULTITHREADING or disable BUILD_TESTS -- or implement the missing feature :)
#       endif /* defined(LIGHTSHIP_UTIL_PLATFORM_LINUX) || defined(LIGHTSHIP_UTIL_PLATFORM_MACOSX) */

    static MUTEX mutex;

#   else /* defined(ENABLE_MULTITHREADING) || defined(ENABLE_MEMORY_EXPLICIT_MALLOC_FAILS) */
#       define mutex
#       define MUTEX_LOCK(x)
#       define MUTEX_UNLOCK(x)
#       define MUTEX_INIT(x)
#       define MUTEX_DEINIT(x)
#   endif /* defined(ENABLE_MULTITHREADING) || defined(ENABLE_MEMORY_EXPLICIT_MALLOC_FAILS) */

struct report_info_t
{
    uintptr_t location;
    uintptr_t size;
#   ifdef ENABLE_MEMORY_BACKTRACE
    int backtrace_size;
    char** backtrace;
#   endif
};

/* ------------------------------------------------------------------------- */
void
memory_init(void)
{
    allocations = 0;
    deallocations = 0;
    ignore_map_malloc = 0;
    map_init_map(&report);
    MUTEX_INIT(mutex)
#   ifdef ENABLE_MEMORY_EXPLICIT_MALLOC_FAILS
    malloc_fail_counter = 0;
#   endif
}

/* ------------------------------------------------------------------------- */
void*
custom_malloc_debug(intptr_t size)
{
    void* p = NULL;
    struct report_info_t* info = NULL;

    MUTEX_LOCK(mutex);

    /* breaking from this will clean up and return NULL */
    for(;;)
    {

#   ifdef ENABLE_MEMORY_EXPLICIT_MALLOC_FAILS
        if(malloc_fail_counter && !ignore_map_malloc)
        {
            /* fail when counter reaches 1 */
            if(malloc_fail_counter == 1)
                break;
            else
                --malloc_fail_counter;
        }
#   endif

        /* allocate */
        p = malloc(size);
        if(p)
            ++allocations;
        else
            break;

        /*
        * Record allocation info. Call to map may allocate memory,
        * so set flag to ignore the call to malloc() when inserting.
        */
        if(!ignore_map_malloc)
        {
            ignore_map_malloc = 1;
            info = (struct report_info_t*)malloc(sizeof(struct report_info_t));
            if(!info)
            {
                fprintf(stderr, "[memory] ERROR: malloc() for report_info_t failed"
                    " -- not enough memory.\n");
                ignore_map_malloc = 0;
                break;
            }

            /* record the location and size of the allocation */
            info->location = (uintptr_t)p;
            info->size = size;

            /* if enabled, generate a backtrace so we know where memory leaks
            * occurred */
#   ifdef ENABLE_MEMORY_BACKTRACE
            if(!(info->backtrace = get_backtrace(&info->backtrace_size)))
                printf("[memory] WARNING: Failed to generate backtrace\n");
#   endif

            /* insert into map */
            if(!map_insert(&report, (uintptr_t)p, info))
            {
                fprintf(stderr,
                "[memory] WARNING: Hash collision occurred when inserting\n"
                "into memory report map. On 64-bit systems the pointers are\n"
                "rounded down to 32-bit unsigned integers, so even though\n"
                "it's rare, collisions can happen.\n\n"
                "The matching call to FREE() will generate a warning saying\n"
                "something is being freed that was never allocated. This is to\n"
                "be expected and can be ignored.\n");
#   ifdef ENABLE_MEMORY_BACKTRACE
                {
                    char** bt;
                    int bt_size, i;
                    if((bt = get_backtrace(&bt_size)))
                    {
                        printf("  backtrace to where malloc() was called:\n");
                        for(i = 0; i < bt_size; ++i)
                            printf("      %s\n", bt[i]);
                        printf("  -----------------------------------------\n");
                        free(bt);
                    }
                    else
                        fprintf(stderr, "[memory] WARNING: Failed to generate backtrace\n");
                }
#   endif
            }
            ignore_map_malloc = 0;
        }

        /* success */
        MUTEX_UNLOCK(mutex)

        return p;
    }

    /* failure */
    if(p)
    {
        free(p);
        --allocations;
    }

    if(info)
    {
#   ifdef ENABLE_MEMORY_BACKTRACE
        if(info->backtrace)
            free(info->backtrace);
#   endif
        free(info);
    }

    MUTEX_UNLOCK(mutex)

    return NULL;
}

/* ------------------------------------------------------------------------- */
void
free_debug(void* ptr)
{
    MUTEX_LOCK(mutex)

    /* find matching allocation and remove from map */
    if(!ignore_map_malloc)
    {
        struct report_info_t* info = (struct report_info_t*)map_erase(&report, (uintptr_t)ptr);
        if(info)
        {
#   ifdef ENABLE_MEMORY_BACKTRACE
            free(info->backtrace);
#   endif
            free(info);
        }
        else
        {
#   ifdef ENABLE_MEMORY_BACKTRACE
            char** bt;
            int bt_size, i;
            printf("  -----------------------------------------\n");
#   endif
            printf("  WARNING: Freeing something that was never allocated\n");
#   ifdef ENABLE_MEMORY_BACKTRACE
            if((bt = get_backtrace(&bt_size)))
            {
                printf("  backtrace to where free() was called:\n");
                for(i = 0; i < bt_size; ++i)
                    printf("      %s\n", bt[i]);
                printf("  -----------------------------------------\n");
                free(bt);
            }
            else
                fprintf(stderr, "[memory] WARNING: Failed to generate backtrace\n");
#   endif
        }
    }

    if(ptr)
    {
        ++deallocations;
        free(ptr);
    }
    else
        fprintf(stderr, "Warning: free(NULL)\n");

    MUTEX_UNLOCK(mutex)
}

/* ------------------------------------------------------------------------- */
void
memory_deinit(void)
{
    --allocations; /* this is the single allocation still held by the report vector */

    printf("=========================================\n");
    printf("Memory Report\n");
    printf("=========================================\n");

    /* report details on any allocations that were not de-allocated */
    if(report.vector.count != 0)
    {
        { MAP_FOR_EACH(&report, struct report_info_t, key, info)
        {
            printf("  un-freed memory at %p, size %p\n", (void*)info->location, (void*)info->size);
            mutated_string_and_hex_dump((void*)info->location, info->size);

#   ifdef ENABLE_MEMORY_BACKTRACE
            printf("  Backtrace to where malloc() was called:\n");
            {
                intptr_t i;
                for(i = BACKTRACE_OMIT_COUNT; i < info->backtrace_size; ++i)
                    printf("      %s\n", info->backtrace[i]);
            }
            free(info->backtrace); /* this was allocated when malloc() was called */
            printf("  -----------------------------------------\n");
#   endif
            free(info);
        }}
        printf("=========================================\n");
    }

    /* overall report */
    printf("allocations: %lu\n", allocations);
    printf("deallocations: %lu\n", deallocations);
    printf("memory leaks: %lu\n", (allocations > deallocations ? allocations - deallocations : deallocations - allocations));
    printf("=========================================\n");

    ++allocations; /* this is the single allocation still held by the report vector */
    ignore_map_malloc = 1;
    map_clear_free(&report);

    MUTEX_DEINIT(mutex)
}

#   ifdef ENABLE_MEMORY_EXPLICIT_MALLOC_FAILS
/* ------------------------------------------------------------------------- */
void
force_malloc_fail_on(void)
{
    force_malloc_fail_after(1);
}

/* ------------------------------------------------------------------------- */
void
force_malloc_fail_after(int num_allocations)
{
    assert(num_allocations > 0);
    MUTEX_LOCK(mutex);
    malloc_fail_counter = num_allocations;
}

/* ------------------------------------------------------------------------- */
void
force_malloc_fail_off(void)
{
    malloc_fail_counter = 0;
    MUTEX_UNLOCK(mutex);
}
#   endif /* ENABLE_MEMORY_EXPLICIT_MALLOC_FAILS */

#else /* ENABLE_MEMORY_REPORT */

void memory_init(void) {}
void memory_deinit(void) {}

#endif /* ENABLE_MEMORY_REPORT */

/* ------------------------------------------------------------------------- */
void
mutated_string_and_hex_dump(void* data, intptr_t length_in_bytes)
{
    char* dump;
    intptr_t i;

    /* allocate and copy data into new buffer */
    if(!(dump = malloc(length_in_bytes + 1)))
    {
        fprintf(stderr, "[memory] WARNING: Failed to malloc() space for dump\n");
        return;
    }
    memcpy(dump, data, length_in_bytes);
    dump[length_in_bytes] = '\0';

    /* mutate null terminators into dots */
    for(i = 0; i != length_in_bytes; ++i)
        if(dump[i] == '\0')
            dump[i] = '.';

    /* dump */
    printf("  mutated string dump: %s\n", dump);
    printf("  hex dump: ");
    for(i = 0; i != length_in_bytes; ++i)
        printf(" %02x", (unsigned char)dump[i]);
    printf("\n");

    free(dump);
}
