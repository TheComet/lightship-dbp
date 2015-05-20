#include "util/pstdint.h"
#include "framework/se_api.h"

void
main_loop_init(void);

void
main_loop_reset_timer(void);

int64_t
main_loop_get_elapsed_time(void);

void
main_loop_do_loop(void);

#ifdef _DEBUG
    EVENT_LISTENER(on_stats);
#endif
