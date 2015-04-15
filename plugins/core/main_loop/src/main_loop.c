#include "plugin_main_loop/config.h"
#include "plugin_main_loop/events.h"
#include "plugin_main_loop/glob.h"
#include "framework/log.h"
#include "util/time.h"
#include <stdio.h>

/* ------------------------------------------------------------------------- */
void
main_loop_init(struct main_loop_t* loop)
{
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
is_time_to_update(struct glob_t* g)
{
    struct main_loop_t* loop = &g->main_loop;
    int64_t elapsed_time = main_loop_get_elapsed_time(loop);

    /* update internal statistics every second */
    if(elapsed_time - loop->statistics.last_update >= 1000000)
    {
        /* calculate render frame rate and update frame rate */
        loop->statistics.render_frame_rate = loop->statistics.render_counter_rel;
        loop->statistics.update_frame_rate = loop->statistics.update_counter_rel;
        loop->statistics.render_counter_rel = 0;
        loop->statistics.update_counter_rel = 0;
        
        /* reset timer */
        loop->statistics.last_update = elapsed_time;
        EVENT_FIRE_FROM_TEMP2(evt_stats, g->events.stats, loop->statistics.render_frame_rate, loop->statistics.update_frame_rate);
    }

    /* calling this function means a render update occurred */
    
    
    /*
     * If time that has passed is smaller than the time that should have passed,
     * it's not time to update yet.
     */
    if(elapsed_time < loop->update_loop_counter * loop->time_between_frames)
        return 0;
    
    /* game loop needs to be updated, increment counter and return non-zero */
    ++loop->update_loop_counter;
    ++loop->statistics.update_counter_rel;
    return 1;
}

/* ------------------------------------------------------------------------- */
void
main_loop_reset_timer(struct main_loop_t* loop)
{
    loop->update_loop_counter = 0;
    loop->time_begin = get_time_in_microseconds();
    loop->statistics.last_update = 0;
}

/* ------------------------------------------------------------------------- */
int64_t
main_loop_get_elapsed_time(struct main_loop_t* loop)
{
    return get_time_in_microseconds() - loop->time_begin;
}

/* ------------------------------------------------------------------------- */
SERVICE(main_loop_start)
{
    struct glob_t* g = get_global(service->game);
    main_loop_reset_timer(&g->main_loop);
    g->main_loop.is_looping = 1;
}

/* ------------------------------------------------------------------------- */
SERVICE(main_loop_stop)
{
    struct glob_t* g = get_global(service->game);
    EVENT_FIRE_FROM_TEMP0(evt_stop, g->events.stop);
}

/* ------------------------------------------------------------------------- */
SERVICE(main_loop_step)
{
    struct glob_t* g = get_global(service->game);
    
    if(g->main_loop.is_looping)
    {
        int updates = 0;
        
        /* dispatch render event */
        EVENT_FIRE_FROM_TEMP0(evt_render, g->events.render);
        ++g->main_loop.statistics.render_counter_rel;
        
        /* dispatch game loop event */
        while(is_time_to_update(g))
        {
            EVENT_FIRE_FROM_TEMP0(evt_update, g->events.update);
            if(++updates >= 20) /* don't allow more than 20 update loops without
                                   a render update */
                break;
        }
    }
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
    char render_str[sizeof(int)*8+1];
    char update_str[sizeof(int)*8+1];
    sprintf(render_str, "%d", render_frame_rate);
    sprintf(update_str, "%d", update_frame_rate);
    
    llog(LOG_INFO, event->game, PLUGIN_NAME, 4,
         "render fps: ", render_str, ", update fps: ", update_str);
}
#endif
