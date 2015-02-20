#include "util/pstdint.h"
#include "util/config.h"

typedef enum file_opts_e
{
    FILE_BINARY=1
} file_opts_e;

LIGHTSHIP_UTIL_PUBLIC_API uint32_t
file_load_into_memory(const char* file_name, void** buffer, file_opts_e opts);

LIGHTSHIP_UTIL_PUBLIC_API void
free_file(void* ptr);
