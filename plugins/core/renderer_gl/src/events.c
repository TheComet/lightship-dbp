#include "util/event_api.h"

#include "glfw3.h"

EVENT_IMPL(evt_close_window);

EVENT_LISTENER(on_render)
{
    /* render everything */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(g_window->window);

    glfwPollEvents();

    /* fire close window event */
    if(glfwGetKey(g_window->window, GLFW_KEY_ESCAPE) == GLFW_PRESS || 
        glfwWindowShouldClose(g_window->window) != 0)
        EVENT_FIRE(evt_close_window, NULL);
}
