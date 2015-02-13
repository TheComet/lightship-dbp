#include "util/pstdint.h"
#include "plugin_manager/events.h"

struct mouse_position_t
{
    double x;
    double y;
};

struct mouse_t
{
    struct mouse_position_t position;
    char state;
};

EVENT_LISTENER2(on_mouse_move, double x, double y);
EVENT_LISTENER1(on_mouse_button_press, uint32_t btn);
EVENT_LISTENER1(on_mouse_button_release, uint32_t btn);
