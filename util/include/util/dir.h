#ifndef LIGHTSHIP_UTIL_DIR_H
#define LIGHTSHIP_UTIL_DIR_H

#include "util/config.h"

struct list_t;

/*!
 * @brief Scans a specified directory and saves the listings in the specified
 * list.
 * @note If the list is not empty, then the data is simply appended.
 * @note The string entries are allocated with MALLOC() and must be FREEd
 * manually when erasing the list.
 * @param[out] list The list to store the directory listings in.
 * @param[in] dir The directory to scan (relative or absolute).
 */
LIGHTSHIP_UTIL_PUBLIC_API void
get_directory_listing(struct list_t* list, const char* dir);

#endif /* LIGHTSHIP_UTIL_DIR_H */
