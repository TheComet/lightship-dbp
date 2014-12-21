#include "main_loop/main_loop.h"
#include "main_loop/events.h"
#include "lightship/api.h"
#include "util/event_api.h"
#include <stdio.h>

static char is_looping = 0;

void main_loop_start(void)
{
    is_looping = 1;
    while(is_looping)
    {
        EVENT_FIRE(evt_render, NULL)
    }
}

void main_loop_stop(struct event_t* evt, void* args)
{
    is_looping = 0;
}
