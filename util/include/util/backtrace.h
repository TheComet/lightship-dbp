#ifndef LIGHTSHIP_UTIL_BACKTRACE_H
#define LIGHTSHIP_UTIL_BACKTRACE_H

#define BACKTRACE_SIZE 64

#include "util/pstdint.h"
#include "util/config.h"

LIGHTSHIP_PUBLIC_API char** get_backtrace(intptr_t* size);

#endif /* LIGHTSHIP_UTIL_BACKTRACE_H */
