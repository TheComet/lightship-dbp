#include "util/memory.h"
#include "util/map.h"
#include "util/backtrace.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define BACKTRACE_OMIT_COUNT 2

#ifdef ENABLE_MEMORY_DEBUGGING
static uintptr_t allocations = 0;
static uintptr_t deallocations = 0;
static uintptr_t ignore_map_malloc = 0;
static struct map_t report;

#   ifdef ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES
static volatile int malloc_fail_counter = 0;
#   endif /* ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES */

/* need a mutex to make malloc_wrapper() and free_wrapper() thread safe */
/* NOTE: Mutex must be recursive */
#   if defined(ENABLE_MULTITHREADING) || defined(ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES)
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
#			include <Windows.h>
#			include <process.h>
#           define MUTEX HANDLE
#			define MUTEX_LOCK(x) WaitForSingleObject(x, INFINITE);
#			define MUTEX_UNLOCK(x) ReleaseMutex(x);
#			define MUTEX_INIT(x) do { x = CreateMutex(NULL, FALSE, NULL); } while(0);
#			define MUTEX_DEINIT(x) CloseHandle(x);
#       endif /* defined(LIGHTSHIP_UTIL_PLATFORM_LINUX) || defined(LIGHTSHIP_UTIL_PLATFORM_MACOSX) */

	static MUTEX mutex;

#   else /* defined(ENABLE_MULTITHREADING) || defined(ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES) */
#       define mutex
#       define MUTEX_LOCK(x)
#       define MUTEX_UNLOCK(x)
#       define MUTEX_INIT(x)
#       define MUTEX_DEINIT(x)
#   endif /* defined(ENABLE_MULTITHREADING) || defined(ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES) */

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
#   ifdef ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES
	malloc_fail_counter = 0;
#   endif
}

/* ------------------------------------------------------------------------- */
void*
malloc_wrapper(intptr_t size)
{
	void* p = NULL;
	struct report_info_t* info = NULL;

	MUTEX_LOCK(mutex);

	/* breaking from this will clean up and return NULL */
	for(;;)
	{

#   ifdef ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES
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
				fprintf(stderr, "[memory] WARNING: Failed to generate backtrace\n");
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
free_wrapper(void* ptr)
{
	MUTEX_LOCK(mutex)

	/* find matching allocation and remove from map */
	if(!ignore_map_malloc)
	{
		struct report_info_t* info = (struct report_info_t*)map_erase(&report, (uintptr_t)ptr);
		if(info)
		{
#   ifdef ENABLE_MEMORY_BACKTRACE
			if(info->backtrace)
				free(info->backtrace);
			else
				fprintf(stderr, "[memory] WARNING: free(): Allocation didn't "
					"have a backtrace (it was NULL)\n");
#   endif
			free(info);
		}
		else
		{
#   ifdef ENABLE_MEMORY_BACKTRACE
			char** bt;
			int bt_size, i;
			fprintf(stderr, "  -----------------------------------------\n");
#   endif
			fprintf(stderr, "  WARNING: Freeing something that was never allocated\n");
#   ifdef ENABLE_MEMORY_BACKTRACE
			if((bt = get_backtrace(&bt_size)))
			{
				fprintf(stderr, "  backtrace to where free() was called:\n");
				for(i = 0; i < bt_size; ++i)
					fprintf(stderr, "      %s\n", bt[i]);
				fprintf(stderr, "  -----------------------------------------\n");
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
uintptr_t
memory_deinit(void)
{
	uintptr_t leaks;

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
	leaks = (allocations > deallocations ? allocations - deallocations : deallocations - allocations);
#if SIZEOF_VOID_PTR == 8
#   define FORMAT_UINTPTR_T "lu"
#elif SIZEOF_VOID_PTR == 4
#   define FORMAT_UINTPTR_T "u"
#endif
	printf("allocations: %" FORMAT_UINTPTR_T "\n", allocations);
	printf("deallocations: %" FORMAT_UINTPTR_T"\n", deallocations);
	printf("memory leaks: %" FORMAT_UINTPTR_T "\n", leaks);
	printf("=========================================\n");

	++allocations; /* this is the single allocation still held by the report vector */
	ignore_map_malloc = 1;
	map_clear_free(&report);

	MUTEX_DEINIT(mutex)

	return leaks;
}

#   ifdef ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES
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
#   endif /* ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES */

#else /* ENABLE_MEMORY_DEBUGGING */

void memory_init(void) {}
uintptr_t memory_deinit(void) { return 0; }

#endif /* ENABLE_MEMORY_DEBUGGING */

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
