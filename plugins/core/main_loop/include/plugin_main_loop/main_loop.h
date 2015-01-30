#include "util/pstdint.h"
#include "util/event_api.h"

struct event_t;
struct event_args;

struct main_loop_statistics_t
{
    int64_t last_update;
    uint32_t update_counter_rel;
    uint32_t render_counter_rel;
    uint32_t update_frame_rate;
    uint32_t render_frame_rate;
};

struct main_loop_t
{
    char is_looping;
    uint32_t fps;
    int64_t time_begin; /* NOTE all times are in microseconds */
    int64_t time_between_frames;
    int64_t update_loop_counter;
    struct main_loop_statistics_t statistics;
};

SERVICE(main_loop_start);

void
main_loop_stop(void);

void
main_loop_reset_timer(void);

int64_t
main_loop_get_elapsed_time(void);

EVENT_LISTENER0(on_main_loop_stop);

#ifdef _DEBUG
#   include "util/event_api.h"
    EVENT_LISTENER2(on_stats, uint32_t render_frame_rate, uint32_t update_frame_rate);
#endif
