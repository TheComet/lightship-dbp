#include "plugin_renderer_gl/events.h"
#include "util/event_api.h"
#include "util/pstdint.h"
#include "glfw3.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    uint32_t k = (uint32_t)key;
    if(action == GLFW_PRESS)
    {
        EVENT_FIRE1(evt_key_press, (uint32_t)k);
    }
    if(action == GLFW_RELEASE)
    {
        EVENT_FIRE1(evt_key_release, (uint32_t)k);
    }
}

void mouse_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    EVENT_FIRE2(evt_mouse_move, (uint32_t)xpos, (uint32_t)ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        EVENT_FIRE1(evt_mouse_button_press, (uint32_t)button);
    }
    if(action == GLFW_RELEASE)
    {
        EVENT_FIRE1(evt_mouse_button_release, (uint32_t)button);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    EVENT_FIRE2(evt_mouse_scroll, (uint32_t)xoffset, (uint32_t)yoffset);
}
