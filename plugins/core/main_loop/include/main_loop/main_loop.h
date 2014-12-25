struct event_t;

struct main_loop_statistics_t
{
    long last_update;
    long update_counter_rel;
    long render_counter_rel;
    unsigned int update_frame_rate;
    unsigned int render_frame_rate;
};

struct main_loop_t
{
    char is_looping;
    unsigned int fps;
    long time_begin; /* NOTE all times are in microseconds */
    long time_between_frames;
    long update_loop_counter;
    struct main_loop_statistics_t statistics;
};

void main_loop_start(void);
void main_loop_stop(struct event_t* evt, void* args);
void main_loop_reset_timer(void);
long main_loop_get_elapsed_time(void);

#include "util/event_api.h"
EVENT_LISTENER(on_stats, struct main_loop_statistics_t* stats);
