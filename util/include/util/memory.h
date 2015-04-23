#ifndef LIGHTSHIP_UTIL_MEMORY_H
#define LIGHTSHIP_UTIL_MEMORY_H

#include "util/pstdint.h"
#include "util/config.h"

C_HEADER_BEGIN

#ifdef ENABLE_MEMORY_REPORT
#   define MALLOC malloc_debug
#   define FREE free_debug
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
 */
LIGHTSHIP_UTIL_PUBLIC_API void
memory_deinit(void);

#ifdef ENABLE_MEMORY_REPORT
/*!
 * @brief Does the same thing as a normal call to malloc(), but does some
 * additional work monitor and track down memory leaks.
 */
LIGHTSHIP_UTIL_PUBLIC_API void*
malloc_debug(intptr_t size);

/*!
 * @brief Does the same thing as a normal call to fee(), but does some
 * additional work monitor and track down memory leaks.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
free_debug(void* ptr);
#endif

void
mutated_string_and_hex_dump(void* data, intptr_t size_in_bytes);

C_HEADER_END

#endif /* LIGHTSHIP_UTIL_MEMORY_H */
