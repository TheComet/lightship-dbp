#include "plugin_input/mouse.h"
#include "plugin_input/events.h"
#include <stdio.h>

static struct mouse_t g_mouse = {
    {0, 0},          /* position */
    0                /* state */
};

EVENT_LISTENER2(on_mouse_move, double x, double y)
{
    g_mouse.position.x = x;
    g_mouse.position.y = y;
}

EVENT_LISTENER1(on_mouse_button_press, uint32_t btn)
{
    g_mouse.state |= (1 << btn);
}

EVENT_LISTENER1(on_mouse_button_release, uint32_t btn)
{
    g_mouse.state &= ~(1 << btn);
    EVENT_FIRE3(evt_mouse_clicked, g_mouse.state, g_mouse.position.x, g_mouse.position.y);
}
