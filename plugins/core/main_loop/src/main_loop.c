#include "plugin_main_loop/events.h"
#include "plugin_main_loop/glob.h"
#include "util/log.h"
#include "util/time.h"
#include <stdio.h>

/* ------------------------------------------------------------------------- */
void
main_loop_init(struct game_t* game)
{
    struct main_loop_t* loop = &get_global(game)->main_loop;
    loop->is_looping = 0;
    loop->fps = 60;
    loop->time_begin = 0;
    loop->time_between_frames = 1000000 / 60;
    loop->update_loop_counter = 0;
    loop->statistics.last_update = 0;
    loop->statistics.render_counter_rel = 0;
    loop->statistics.render_frame_rate = 0;
    loop->statistics.update_counter_rel = 0;
    loop->statistics.update_frame_rate = 0;
}

/* ------------------------------------------------------------------------- */
static char
is_time_to_update(struct game_t* game)
{
    int64_t elapsed_time = main_loop_get_elapsed_time(game);

    /* update internal statistics every second */
    if(elapsed_time - get_global(game)->main_loop.statistics.last_update >= 1000000)
    {
        /* calculate render frame rate and update frame rate */
        get_global(game)->main_loop.statistics.render_frame_rate = get_global(game)->main_loop.statistics.render_counter_rel;
        get_global(game)->main_loop.statistics.update_frame_rate = get_global(game)->main_loop.statistics.update_counter_rel;
        get_global(game)->main_loop.statistics.render_counter_rel = 0;
        get_global(game)->main_loop.statistics.update_counter_rel = 0;
        
        /* reset timer */
        get_global(game)->main_loop.statistics.last_update = elapsed_time;
        EVENT_FIRE2(evt_stats, get_global(game)->main_loop.statistics.render_frame_rate, get_global(game)->main_loop.statistics.update_frame_rate);
    }

    /* calling this function means a render update occurred */
    ++get_global(game)->main_loop.statistics.render_counter_rel;
    
    /*
     * If time that has passed is smaller than the time that should have passed,
     * it's not time to update yet.
     */
    if(elapsed_time < get_global(game)->main_loop.update_loop_counter * get_global(game)->main_loop.time_between_frames)
        return 0;
    
    /* game loop needs to be updated, increment counter and return non-zero */
    ++get_global(game)->main_loop.update_loop_counter;
    ++get_global(game)->main_loop.statistics.update_counter_rel;
    return 1;
}

/* ------------------------------------------------------------------------- */
void
main_loop_reset_timer(struct game_t* game)
{
    get_global(game)->main_loop.update_loop_counter = 0;
    get_global(game)->main_loop.time_begin = get_time_in_microseconds();
    get_global(game)->main_loop.statistics.last_update = 0;
}

/* ------------------------------------------------------------------------- */
int64_t
main_loop_get_elapsed_time(struct game_t* game)
{
    return get_time_in_microseconds() - get_global(game)->main_loop.time_begin;
}

/* ------------------------------------------------------------------------- */
SERVICE(main_loop_start)
{
    main_loop_reset_timer(service->game);
    get_global(service->game)->main_loop.is_looping = 1;
    while(get_global(service->game)->main_loop.is_looping)
    {
        int updates = 0;
        
        /* dispatch render event */
        EVENT_FIRE0(evt_render);
        
        /* dispatch game loop event */
        while(is_time_to_update(service->game))
        {
            EVENT_FIRE0(evt_update);
            if(++updates >= 10) /* don't allow more than 10 update loops without
                                   a render update */
                break;
        }
    }
}

/* ------------------------------------------------------------------------- */
SERVICE(main_loop_stop)
{
    EVENT_FIRE0(evt_stop);
}

/* ------------------------------------------------------------------------- */
EVENT_LISTENER0(on_main_loop_stop)
{
    get_global(event->game)->main_loop.is_looping = 0;
}

/* ------------------------------------------------------------------------- */
#ifdef _DEBUG
EVENT_LISTENER2(on_stats, uint32_t render_frame_rate, uint32_t update_frame_rate)
{
    /*printf("render fps: %u, update fps: %u\n", render_frame_rate, update_frame_rate);*/
}
#endif
