#include "util/memory.h"
#include "util/map.h"
#include "util/backtrace.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BACKTRACE_OMIT_COUNT 2

#ifdef ENABLE_MEMORY_REPORT
static intptr_t allocations = 0;
static intptr_t deallocations = 0;
static intptr_t ignore_map_malloc = 0;
static struct map_t report;

/* need a mutex to make malloc_debug() and free_debug() thread safe */
/* NOTE: Mutex must be recursive */
#ifdef ENABLE_MULTITHREADING
    /* linux or mac */
#   if defined(LIGHTSHIP_UTIL_PLATFORM_LINUX) || defined(LIGHTSHIP_UTIL_PLATFORM_MACOSX)
#       include <pthread.h>
#       define MUTEX pthread_mutex_t
#       define MUTEX_LOCK(x) pthread_mutex_lock(&(x));
#       define MUTEX_UNLOCK(x) pthread_mutex_unlock(&(x));
#       define MUTEX_INIT(x) do {                                           \
            pthread_mutexattr_t attr;                                       \
            pthread_mutexattr_init(&attr);                                  \
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);      \
            pthread_mutex_init(&(x), &attr);                                \
            pthread_mutexattr_destroy(&attr);                               \
        } while(0);

#   else
#       error Dont know how to create a mutex for the target platform. Either disable ENABLE_MULTITHREADING or implement the missing feature.
#   endif
    static MUTEX mutex;
#else /* ENABLE_MULTITHREADING */
#   define mutex
#   define MUTEX_LOCK(x)
#   define MUTEX_UNLOCK(x)
#   define MUTEX_INIT(x)
#endif /* ENABLE_MULTITHREADING */

struct report_info_t
{
    intptr_t location;
    intptr_t size;
#ifdef ENABLE_MEMORY_BACKTRACE
    int backtrace_size;
    char** backtrace;
#endif
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
}

/* ------------------------------------------------------------------------- */
void*
malloc_debug(intptr_t size)
{
    void* p = malloc(size);

    MUTEX_LOCK(mutex)

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
#ifdef ENABLE_MEMORY_BACKTRACE
        info->backtrace = get_backtrace(&info->backtrace_size);
#endif
        ignore_map_malloc = 1;
        map_insert(&report, (intptr_t)p, info);
        ignore_map_malloc = 0;
    }

    MUTEX_UNLOCK(mutex)

    return p;
}

/* ------------------------------------------------------------------------- */
void
free_debug(void* ptr)
{
    MUTEX_LOCK(mutex)

    /* remove the memory location from the vector */
    if(!ignore_map_malloc)
    {
        /*struct report_info_t* info = (struct report_info_t*)map_find(&report, (intptr_t)ptr);*/
        int success = 0;
        struct report_info_t* info = map_find(&report, (intptr_t)ptr);
        if(info)
        {
#ifdef ENABLE_MEMORY_BACKTRACE
            free(info->backtrace);
#endif
            map_erase(&report, info->location);
            free(info);
            success = 1;
        }

        if(!success)
        {
#ifdef ENABLE_MEMORY_BACKTRACE
            char** bt;
            int bt_size, i;
            printf("  -----------------------------------------\n");
#endif
            printf("  WARNING: Freeing something that was never allocated\n");
#ifdef ENABLE_MEMORY_BACKTRACE
            bt = get_backtrace(&bt_size);
            printf("  backtrace to where free() was called:\n");
            for(i = 0; i < bt_size; ++i)
                printf("      %s\n", bt[i]);
            printf("  -----------------------------------------\n");
            free(bt);
#endif
        }
    }

    if(ptr)
        ++deallocations;
    else
        fprintf(stderr, "Warning: free(NULL)\n");

    MUTEX_UNLOCK(mutex)

    free(ptr);
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

#ifdef ENABLE_MEMORY_BACKTRACE
            printf("  Backtrace to where malloc() was called:\n");
            {
                intptr_t i;
                for(i = BACKTRACE_OMIT_COUNT; i < info->backtrace_size; ++i)
                    printf("      %s\n", info->backtrace[i]);
            }
            free(info->backtrace); /* this was allocated when malloc() was called */
            printf("  -----------------------------------------\n");
#endif
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
}
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
    dump = malloc(length_in_bytes + 1);
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
