#ifndef LIGHTSHIP_UTIL_FILE_H
#define LIGHTSHIP_UTIL_FILE_H

#include "util/pstdint.h"
#include "util/config.h"

C_HEADER_BEGIN

typedef enum file_opts_e
{
	FILE_BINARY=1
} file_opts_e;

/*!
 * @brief Loads the contents of a file into memory.
 * @param[in] file_name The file to load.
 * @param[out] buffer This will point to the memory location of the loaded file
 * when successful.
 * @param opts File open options.
 * @return Returns the size of the buffer in bytes. If anything fails, 0 is
 * returned.
 */
LIGHTSHIP_UTIL_PUBLIC_API uint32_t
file_load_into_memory(const char* file_name, void** buffer, file_opts_e opts);

/*!
 * @brief Frees the buffer allocated by a previous call to
 * file_load_into_memory().
 * @param ptr The pointer to the buffer to free.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
free_file(void* ptr);

C_HEADER_END

#endif /* LIGHTSHIP_UTIL_FILE_H */
