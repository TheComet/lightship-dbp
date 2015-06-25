#include "plugin_input/mouse.h"
#include "plugin_input/events.h"
#include "framework/log.h"
#include <stdio.h>

/* ------------------------------------------------------------------------- */
static struct mouse_t g_mouse = {
	{0, 0},          /* position */
	0                /* state */
};

/* ------------------------------------------------------------------------- */
EVENT_LISTENER(on_mouse_move)
{
	EXTRACT_ARGUMENT(0, x, double, double);
	EXTRACT_ARGUMENT(1, y, double, double);
	g_mouse.position.x = x;
	g_mouse.position.y = y;
}

/* ------------------------------------------------------------------------- */
EVENT_LISTENER(on_mouse_button_press)
{
	EXTRACT_ARGUMENT(0, btn, uint32_t, uint32_t);
	g_mouse.state |= (1 << btn);
}

/* ------------------------------------------------------------------------- */
EVENT_LISTENER(on_mouse_button_release)
{
	EXTRACT_ARGUMENT(0, btn, uint32_t, uint32_t);
	g_mouse.state &= ~(1 << btn);
	EVENT_FIRE3(evt_mouse_clicked, g_mouse.state, g_mouse.position.x, g_mouse.position.y);
}
