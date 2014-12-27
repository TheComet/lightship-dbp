#ifndef LIGHTSHIP_UTIL_BACKTRACE_H
#define LIGHTSHIP_UTIL_BACKTRACE_H

#define BACKTRACE_SIZE 64

#include "util/pstdint.h"
#include "util/config.h"

/*!
 * @brief Generates a backtrace.
 * @param[in] size The maximum number of frames to walk.
 * @return Returns an array of char* arrays.
 * @note The returned array must be freed manually with FREE(returned_array).
 */
LIGHTSHIP_PUBLIC_API char** get_backtrace(intptr_t* size);

#endif /* LIGHTSHIP_UTIL_BACKTRACE_H */
