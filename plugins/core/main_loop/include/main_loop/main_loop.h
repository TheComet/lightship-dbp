#include "util/pstdint.h"

struct event_t;
struct event_args;

struct main_loop_statistics_t
{
    uint64_t last_update;
    unsigned long update_counter_rel;
    unsigned long render_counter_rel;
    unsigned int update_frame_rate;
    unsigned int render_frame_rate;
};

struct main_loop_t
{
    char is_looping;
    unsigned int fps;
    uint64_t time_begin; /* NOTE all times are in microseconds */
    uint64_t time_between_frames;
    uint64_t update_loop_counter;
    struct main_loop_statistics_t statistics;
};

void main_loop_start(void);
void main_loop_stop(struct event_t* evt, void* args);
void main_loop_reset_timer(void);
uint64_t main_loop_get_elapsed_time(void);

#ifdef _DEBUG
#   include "util/event_api.h"
    EVENT_LISTENER(on_stats, struct main_loop_statistics_t* stats);
#endif
