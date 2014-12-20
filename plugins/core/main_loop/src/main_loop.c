#include "main_loop/main_loop.h"

static char is_looping = 0;

void main_loop_start(void)
{
    is_looping = 1;
    while(is_looping)
    {
    }
}

void main_loop_stop(void)
{
    is_looping = 0;
}
