#ifndef LIGHTSHIP_UTIL_STRING_H
#define LIGHTSHIP_UTIL_STRING_H

#include <stdio.h>
#include "util/pstdint.h"
#include "util/config.h"

/*!
 * @brief Concatenates n number of strings and prints it to stdout.
 * @param[in] num_strs The number of strings that should be concatenated.
 * @param[in] ...str A list of comma-seperated strings to concatenate.
 */
LIGHTSHIP_PUBLIC_API void
stdout_strings(uint32_t num_strs, ...);

/*!
 * @brief Concatenates n number of strings and prints it to stderr.
 * @param[in] num_strs The number of strings that should be concatenated.
 * @param[in] ...str A list of comma-seperated strings to concatenate.
 */
LIGHTSHIP_PUBLIC_API void
stderr_strings(uint32_t num_strs, ...);

/*!
 * @brief Concatenates n number of strings and returns the buffer.
 * @note The buffer needs to be FREEd manually when it is no longer required.
 * @param[in] num_strs The number of strings that should be concatenated.
 * @param[in] ...str A list of comma-seperated strings to concatenate.
 * @return The new buffer containing all concatenated strings.
 */
LIGHTSHIP_PUBLIC_API char*
cat_strings(uint32_t num_strs, ...);

/*!
 * @brief Copies the specified into a new buffer and returns it.
 * @note The buffer needs to be FREEd manually when it is no longer required.
 * @param[in] str The string to copy.
 * @return The new buffer containing a copy of the input string.
 */
LIGHTSHIP_PUBLIC_API char*
malloc_string(const char* str);

/*!
 * @brief Checks if a character is a number or not.
 * @return Returns 1 if it is a number, 0 if otherwise.
 */
LIGHTSHIP_PUBLIC_API char
is_number(const char chr);

#endif /* LIGHTSHIP_UTIL_STRING_H */
