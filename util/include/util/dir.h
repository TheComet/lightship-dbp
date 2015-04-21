#ifndef LIGHTSHIP_UTIL_DIR_H
#define LIGHTSHIP_UTIL_DIR_H

#include "util/config.h"

C_HEADER_BEGIN

struct list_t;

/*!
 * @brief Scans a specified directory and saves the listings in the specified
 * list.
 * @note If the list is not empty, then the data is simply appended.
 * @note The string entries are allocated with string functions and must be
 * manually de-allocated with free_string() when erasing the list.
 * @param[out] list The list to store the directory listings in.
 * @param[in] dir The directory to scan (relative or absolute).
 * @return Returns 0 if an error occurs, 1 if otherwise.
 */
LIGHTSHIP_UTIL_PUBLIC_API char
get_directory_listing(struct list_t* list, const char* dir);

C_HEADER_END

#endif /* LIGHTSHIP_UTIL_DIR_H */
