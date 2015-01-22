#ifndef LIGHTSHIP_UTIL_STRING_H
#define LIGHTSHIP_UTIL_STRING_H

#include <stdio.h>
#include "util/pstdint.h"
#include "util/config.h"

C_HEADER_BEGIN

/*!
 * @brief Very important on Windows, so objects that were allocated from within
 * this library are also freed in this library.
 * @param str The string to free.
 */
LIGHTSHIP_PUBLIC_API void
free_string(void* str);

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
 * @note The buffer needs to be freed manually with string_free() when it is no
 * longer required.
 * @param[in] num_strs The number of strings that should be concatenated.
 * @param[in] ...str A list of comma-seperated strings to concatenate.
 * @return The new buffer containing all concatenated strings.
 * @warning Use string_free() to free the returned buffer.
 */
LIGHTSHIP_PUBLIC_API char*
cat_strings(uint32_t num_strs, ...);

/*!
 * @brief Copies the specified into a new buffer and returns it.
 * @note The buffer needs to be freed manually with string_free() when it is no
 * longer required.
 * @param[in] str The string to copy.
 * @return The new buffer containing a copy of the input string.
 * @warning Use util_free() to free the returned buffer.
 */
LIGHTSHIP_PUBLIC_API char*
malloc_string(const char* str);

/*!
 * @brief Concatenates n number of strings and returns the buffer.
 * @note The buffer needs to be freed manually with string_free() when it is no
 * longer required.
 * @param[in] num_strs The number of strings that should be concatenated.
 * @param[in] ...str A list of comma-seperated strings to concatenate.
 * @return The new buffer containing all concatenated strings.
 * @warning Use string_free() to free the returned buffer.
 */
LIGHTSHIP_PUBLIC_API wchar_t*
cat_wstrings(uint32_t num_strs, ...);

/*!
 * @brief Copies the specified into a new buffer and returns it.
 * @note The buffer needs to be freed manually with string_free() when it is no
 * longer required.
 * @param[in] str The string to copy.
 * @return The new buffer containing a copy of the input string.
 * @warning Use util_free() to free the returned buffer.
 */
LIGHTSHIP_PUBLIC_API wchar_t*
malloc_wstring(const wchar_t* wcs);

/*!
 * @brief Checks if a character is a number or not.
 * @return Returns 1 if it is a number, 0 if otherwise.
 */
LIGHTSHIP_PUBLIC_API char
is_number(const char chr);

LIGHTSHIP_PUBLIC_API wchar_t*
strtowcs(const char* str);

LIGHTSHIP_PUBLIC_API char*
wcstostr(wchar_t* wcs);

C_HEADER_END

#endif /* LIGHTSHIP_UTIL_STRING_H */
