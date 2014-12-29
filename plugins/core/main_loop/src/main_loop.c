#include "plugin_main_loop/main_loop.h"
#include "plugin_main_loop/events.h"
#include "lightship/api.h"
#include "util/event_api.h"
#include "util/log.h"
#include "util/time.h"
#include <stdio.h>

static struct main_loop_t loop = {
    0,             /* is looping */
    60,            /* fps */
    0,             /* time begin */
    1000000 / 60,  /* time between frames */
    0,             /* update loop counter */
    {              /* statistics */
        0,
        0,
        0,
        0,
        0
    }
};

static char
is_time_to_update(void)
{
    int64_t elapsed_time = main_loop_get_elapsed_time();

    /* update internal statistics every second */
    if(elapsed_time - loop.statistics.last_update >= 1000000)
    {
        /* calculate render frame rate and update frame rate */
        loop.statistics.render_frame_rate = loop.statistics.render_counter_rel;
        loop.statistics.update_frame_rate = loop.statistics.update_counter_rel;
        loop.statistics.render_counter_rel = 0;
        loop.statistics.update_counter_rel = 0;
        
        /* reset timer */
        loop.statistics.last_update = elapsed_time;
        EVENT_FIRE2(evt_stats, loop.statistics.render_frame_rate, loop.statistics.update_frame_rate);
    }

    /* calling this function means a render update occurred */
    ++loop.statistics.render_counter_rel;
    
    /*
     * If time that has passed is smaller than the time that should have passed,
     * it's not time to update yet.
     */
    if(elapsed_time < loop.update_loop_counter * loop.time_between_frames)
        return 0;
    
    /* game loop needs to be updated, increment counter and return non-zero */
    ++loop.update_loop_counter;
    ++loop.statistics.update_counter_rel;
    return 1;
}

void
main_loop_start(void)
{
    main_loop_reset_timer();
    loop.is_looping = 1;
    while(loop.is_looping)
    {
        int updates = 0;
        
        /* dispatch render event */
        EVENT_FIRE(evt_render)
        
        /* dispatch game loop event */
        while(is_time_to_update())
        {
            EVENT_FIRE(evt_update);
            if(++updates >= 10) /* don't allow more than 10 update loops without
                                   a render update */
                break;
        }
    }
}

void
main_loop_stop(const struct event_t* evt, void* args)
{
    loop.is_looping = 0;
}

void
main_loop_reset_timer(void)
{
    loop.update_loop_counter = 0;
    loop.time_begin = get_time_in_microseconds();
    loop.statistics.last_update = 0;
}

int64_t
main_loop_get_elapsed_time(void)
{
    return get_time_in_microseconds() - loop.time_begin;
}

#ifdef _DEBUG
EVENT_LISTENER2(on_stats, uint32_t render_frame_rate, uint32_t update_frame_rate)
{
    printf("render fps: %u, update fps: %u\n", render_frame_rate, update_frame_rate);
}
#endif
