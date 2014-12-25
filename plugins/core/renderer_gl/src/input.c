#include "renderer_gl/events.h"
#include "util/event_api.h"
#include "glfw3.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    uint32_t k = (uint32_t)key;
    if(action == GLFW_PRESS)
    {
        EVENT_FIRE(evt_key_press, &k);
    }
    if(action == GLFW_RELEASE)
    {
        EVENT_FIRE(evt_key_release, &k);
    }
}

void mouse_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    struct { uint32_t x; uint32_t y; } pos;
    pos.x = (uint32_t)xpos;
    pos.y = (uint32_t)ypos;
    EVENT_FIRE(evt_mouse_move, &pos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    uint32_t btn = (uint32_t)button;
    if(action == GLFW_PRESS)
    {
        EVENT_FIRE(evt_mouse_button_press, &btn);
    }
    if(action == GLFW_RELEASE)
    {
        EVENT_FIRE(evt_mouse_button_release, &btn);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    struct { uint32_t x; uint32_t y; } scroll;
    scroll.x = (uint32_t)xoffset;
    scroll.y = (uint32_t)yoffset;
    EVENT_FIRE(evt_mouse_scroll, &scroll);
}
