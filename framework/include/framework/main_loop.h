#include "util/pstdint.h"
#include "framework/event_api.h"
#include "framework/service_api.h"

EVENT_H0(evt_tick)
EVENT_H0(evt_render)
EVENT_H2(evt_loop_stats, uint32_t, uint32_t)

void
main_loop_init(void);

void
main_loop_reset_timer(void);

int64_t
main_loop_get_elapsed_time(void);

void
main_loop_do_loop(void*);

#ifdef _DEBUG
#   include "framework/event_api.h"
    EVENT_LISTENER2(on_stats, uint32_t render_frame_rate, uint32_t update_frame_rate);
#endif
