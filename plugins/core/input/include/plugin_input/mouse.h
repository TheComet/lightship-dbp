#include "util/pstdint.h"
#include "framework/events.h"

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

EVENT_LISTENER(on_mouse_move);
EVENT_LISTENER(on_mouse_button_press);
EVENT_LISTENER(on_mouse_button_release);
