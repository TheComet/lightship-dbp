#ifndef LIGHTSHIP_UTIL_MEMORY_H
#define LIGHTSHIP_UTIL_MEMORY_H

#include "util/pstdint.h"
#include "util/config.h"

C_HEADER_BEGIN

#ifdef MEMORY_ENABLE_MEMORY_REPORT
#   define MALLOC malloc_debug
#   define FREE free_debug
#else
#   include <stdlib.h>
#   define MALLOC malloc
#   define FREE free
#endif

/*!
 * @brief Initialises the memory system.
 * 
 * In release mode this does nothing. In debug mode it will initialise
 * memory reports and backtraces, if enabled.
 */
LIGHTSHIP_PUBLIC_API void
memory_init(void);

/*!
 * @brief De-initialises the memory system.
 * 
 * In release mode this does nothing. In debug mode this will output the memory
 * report and print backtraces, if enabled.
 */
LIGHTSHIP_PUBLIC_API void
memory_deinit(void);

#ifdef MEMORY_ENABLE_MEMORY_REPORT
/*!
 * @brief Does the same thing as a normal call to malloc(), but does some
 * additional work monitor and track down memory leaks.
 */
LIGHTSHIP_PUBLIC_API void*
malloc_debug(intptr_t size);

/*!
 * @brief Does the same thing as a normal call to fee(), but does some
 * additional work monitor and track down memory leaks.
 */
LIGHTSHIP_PUBLIC_API void
free_debug(void* ptr);
#endif

C_HEADER_END

#endif /* LIGHTSHIP_UTIL_MEMORY_H */
