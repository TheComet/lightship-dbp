#include "util/pstdint.h"
#include "util/events.h"

struct mouse_position_t
{
    uint32_t x;
    uint32_t y;
};

struct mouse_t
{
    struct mouse_position_t position;
    char state;
};

EVENT_LISTENER2(on_mouse_move, uint32_t x, uint32_t y);
EVENT_LISTENER1(on_mouse_button_press, uint32_t btn);
EVENT_LISTENER1(on_mouse_button_release, uint32_t btn);
