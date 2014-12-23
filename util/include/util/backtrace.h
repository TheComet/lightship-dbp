#define BACKTRACE_SIZE 64

#include "util/pstdint.h"

char** get_backtrace(intptr_t* size);
