#ifndef LIGHTSHIP_UTIL_MEMORY_H
#define LIGHTSHIP_UTIL_MEMORY_H

#include "util/pstdint.h"
#include "util/config.h"

C_HEADER_BEGIN

#ifdef ENABLE_MEMORY_DEBUGGING
#   define MALLOC malloc_wrapper
#   define FREE free_wrapper
#else
#   include <stdlib.h>
#   define MALLOC malloc
#   define FREE free
#endif

#define RETURN_NOTHING
#define OUT_OF_MEMORY(where, ret_val) do { \
        llog_critical_use_no_memory("malloc() failed in " where " - not enough memory"); \
        return ret_val; \
    } while(0)

/*!
 * @brief Initialises the memory system.
 *
 * In release mode this does nothing. In debug mode it will initialise
 * memory reports and backtraces, if enabled.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
memory_init(void);

/*!
 * @brief De-initialises the memory system.
 *
 * In release mode this does nothing. In debug mode this will output the memory
 * report and print backtraces, if enabled.
 * @return Returns the number of memory leaks.
 */
LIGHTSHIP_UTIL_PUBLIC_API uintptr_t
memory_deinit(void);

#ifdef ENABLE_MEMORY_DEBUGGING
/*!
 * @brief Does the same thing as a normal call to malloc(), but does some
 * additional work monitor and track down memory leaks.
 */
LIGHTSHIP_UTIL_PUBLIC_API void*
malloc_wrapper(intptr_t size);

/*!
 * @brief Does the same thing as a normal call to fee(), but does some
 * additional work monitor and track down memory leaks.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
free_wrapper(void* ptr);

#   ifdef ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES
/*!
 * @brief Causes the next call to MALLOC() to fail.
 * @note Because unit tests are executed in parallel, this function will
 * acquire a mutex, which will be released again when force_malloc_fail_off()
 * is called.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
force_malloc_fail_on(void);

/*!
 * @brief Causes all calls to malloc() after the nth call to fail.
 * @note Because unit tests are executed in parallel, this function will
 * acquire a mutex, which will be released again when force_malloc_fail_off()
 * is called.
 * @param num_allocations After how many allocations malloc() should begin to
 * fail. 1 means instantly, 2 means after one call, etc.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
force_malloc_fail_after(int num_allocations);

/*!
 * @brief Allows the next call to MALLOC() to function normally again.
 * @note Because unit tests are executed in parallel, this function releases
 * the mutex previously acquired in force_malloc_fail_on().
 */
LIGHTSHIP_UTIL_PUBLIC_API void
force_malloc_fail_off(void);
#   endif /* ENABLE_MEMORY_EXPLICIT_MALLOC_FAILURES */


#endif /* ENABLE_MEMORY_DEBUGGING */

LIGHTSHIP_UTIL_PUBLIC_API void
mutated_string_and_hex_dump(void* data, intptr_t size_in_bytes);

C_HEADER_END

#endif /* LIGHTSHIP_UTIL_MEMORY_H */
