#include "util/time.h"
#include <sys/time.h>
#include <stdio.h>

/* ------------------------------------------------------------------------- */
int64_t
get_time_in_microseconds(void)
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_usec + time.tv_sec*1000000;
}
