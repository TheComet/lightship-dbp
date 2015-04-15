#include "framework/main_loop.h"
#include "framework/events.h"
#include "framework/log.h"
#include "framework/game.h"
#include "util/time.h"
#include <stdio.h>

struct main_loop_statistics_t
{
    int64_t last_tick;
    uint32_t tick_counter_rel;
    uint32_t render_counter_rel;
    uint32_t tick_frame_rate;
    uint32_t render_frame_rate;
};

struct main_loop_t
{
    char is_looping;
    uint32_t fps;
    int64_t time_begin; /* NOTE all times are in microseconds */
    int64_t time_between_ticks;
    int64_t tick_counter;
    struct main_loop_statistics_t statistics;
};

static struct main_loop_t g_loop;

/* ------------------------------------------------------------------------- */
void
main_loop_init(void)
{
    g_loop.is_looping = 0;
    g_loop.fps = 60;
    g_loop.time_begin = 0;
    g_loop.time_between_ticks = 1000000 / 60;
    g_loop.tick_counter = 0;
    g_loop.statistics.last_tick = 0;
    g_loop.statistics.render_counter_rel = 0;
    g_loop.statistics.render_frame_rate = 0;
    g_loop.statistics.tick_counter_rel = 0;
    g_loop.statistics.tick_frame_rate = 0;
}

/* ------------------------------------------------------------------------- */
static char
is_time_to_update(void)
{
    int64_t elapsed_time = main_loop_get_elapsed_time();

    /* update internal statistics every second */
    if(elapsed_time - g_loop.statistics.last_tick >= 1000000)
    {
        /* calculate render frame rate and update frame rate */
        g_loop.statistics.render_frame_rate = g_loop.statistics.render_counter_rel;
        g_loop.statistics.tick_frame_rate = g_loop.statistics.tick_counter_rel;
        g_loop.statistics.render_counter_rel = 0;
        g_loop.statistics.tick_counter_rel = 0;
        
        /* reset timer */
        g_loop.statistics.last_tick = elapsed_time;
        game_dispatch_stats(g_loop.statistics.render_frame_rate, g_loop.statistics.tick_frame_rate);
    }

    /* calling this function means a render update occurred */
    
    
    /*
     * If time that has passed is smaller than the time that should have passed,
     * it's not time to update yet.
     */
    if(elapsed_time < g_loop.tick_counter * g_loop.time_between_ticks)
        return 0;
    
    /* game loop needs to be updated, increment counter and return non-zero */
    ++g_loop.tick_counter;
    ++g_loop.statistics.tick_counter_rel;
    return 1;
}

/* ------------------------------------------------------------------------- */
void
main_loop_reset_timer(void)
{
    g_loop.tick_counter = 0;
    g_loop.time_begin = get_time_in_microseconds();
    g_loop.statistics.last_tick = 0;
}

/* ------------------------------------------------------------------------- */
int64_t
main_loop_get_elapsed_time(void)
{
    return get_time_in_microseconds() - g_loop.time_begin;
}

/* ------------------------------------------------------------------------- */
void
main_loop_do_loop(void)
{
    int updates = 0;
    
    /* dispatch render events */
    game_dispatch_render();
    ++g_loop.statistics.render_counter_rel;
    
    /* dispatch game loop event */
    while(is_time_to_update())
    {
        game_dispatch_tick();
        if(++updates >= 20) /* don't allow more than 20 update loops without
                                a render update */
            break;
    }
}

/* ------------------------------------------------------------------------- */
#ifdef _DEBUG
EVENT_LISTENER2(on_stats, uint32_t render_frame_rate, uint32_t tick_frame_rate)
{
    printf("render fps: %u, update fps: %u\n", render_frame_rate, tick_frame_rate);
}
#endif
