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
LIGHTSHIP_UTIL_PUBLIC_API void
free_string(void* str);

/*!
 * @brief Concatenates n number of strings and prints it to stdout.
 * @param[in] num_strs The number of strings that should be concatenated.
 * @param[in] ...str A list of comma-seperated strings to concatenate.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
stdout_strings(uint32_t num_strs, ...);

/*!
 * @brief Concatenates n number of strings and prints it to stderr.
 * @param[in] num_strs The number of strings that should be concatenated.
 * @param[in] ...str A list of comma-seperated strings to concatenate.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
stderr_strings(uint32_t num_strs, ...);

/*!
 * @brief Concatenates n number of strings and returns the buffer.
 * @note The buffer needs to be freed manually with string_free() when it is no
 * longer required.
 * @param[in] num_strs The number of strings that should be concatenated. Can
 * be 0.
 * @param[in] ...str A list of comma-separated strings to concatenate. Strings
 * may be NULL, in which case they are treated as empty strings.
 * @return The new buffer containing all concatenated strings. Even if all
 * strings are NULL, a buffer will still be allocated and returned.
 * @warning Use string_free() to free the returned buffer.
 */
LIGHTSHIP_UTIL_PUBLIC_API char*
cat_strings(uint32_t num_strs, ...);

/*!
 * @brief Copies the specified into a new buffer and returns it.
 * @note The buffer needs to be freed manually with string_free() when it is no
 * longer required.
 * @param[in] str The string to copy.
 * @return The new buffer containing a copy of the input string.
 * @warning Use util_free() to free the returned buffer.
 */
LIGHTSHIP_UTIL_PUBLIC_API char*
malloc_string(const char* str);

/*!
 * @brief Concatenates n number of strings and returns the buffer.
 * @note The buffer needs to be freed manually with string_free() when it is no
 * longer required.
 * @param[in] num_strs The number of strings that should be concatenated. Can
 * be 0.
 * @param[in] ...str A list of comma-separated strings to concatenate. Strings
 * may be NULL, in which case they are treated as empty strings.
 * @return The new buffer containing all concatenated strings. Even if all
 * strings are NULL, a buffer will still be allocated and returned.
 * @warning Use string_free() to free the returned buffer.
 */
LIGHTSHIP_UTIL_PUBLIC_API wchar_t*
cat_wstrings(uint32_t num_strs, ...);

/*!
 * @brief Copies the specified into a new buffer and returns it.
 * @note The buffer needs to be freed manually with string_free() when it is no
 * longer required.
 * @param[in] str The string to copy.
 * @return The new buffer containing a copy of the input string.
 * @warning Use util_free() to free the returned buffer.
 */
LIGHTSHIP_UTIL_PUBLIC_API wchar_t*
malloc_wstring(const wchar_t* wcs);

LIGHTSHIP_UTIL_PUBLIC_API wchar_t*
strtowcs(const char* str);

LIGHTSHIP_UTIL_PUBLIC_API char*
wcstostr(const wchar_t* wcs);

LIGHTSHIP_UTIL_PUBLIC_API void
crlf2lf(char* src);

C_HEADER_END

#endif /* LIGHTSHIP_UTIL_STRING_H */
