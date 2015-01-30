#include "plugin_renderer_gl/window.h"
#include "plugin_renderer_gl/events.h"
#include "plugin_renderer_gl/input.h"
#include "plugin_renderer_gl/2d.h"
#include "plugin_renderer_gl/text.h"
#include "util/linked_list.h"
#include "util/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GL/glew.h"
#include "glfw3.h"

struct window_t g_window;

char
window_init(void)
{
    GLFWwindow* glfw_window;
    
    memset(&g_window, 0, sizeof(struct window_t));

    /* configure preferences */
    glfwWindowHint(GLFW_SAMPLES, 4); /* 4x antialiasing */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); /* want GL 3.3 */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); /* to make macOS happy */
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); /* don't want the old OpenGL */
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    /* window dimensions */
    g_window.width = 800;
    g_window.height = 600;
    
    /* open window and create OpenGL context */
    glfw_window = glfwCreateWindow(g_window.width, g_window.height, "Light Ship", NULL, NULL);
    if(glfw_window == NULL)
    {
        llog(LOG_ERROR, 1, "Failed to open glfw window. If you have an Intel GPU, they are not 3.3 compatible.");
        return 0;
    }
    glfwMakeContextCurrent(glfw_window); /* initialise GLEW */
    glewExperimental = 1; /* needed in core profile */
    if(glewInit() != GLEW_OK)
    {
        llog(LOG_ERROR, 1, "Failed to initialise GLEW\n");
        return 0;
    }
    glClearColor(0.0, 0.0, 0.4, 0.0);
    
    /* ensure the escape key can be captured */
    glfwSetInputMode(glfw_window, GLFW_STICKY_KEYS, GL_TRUE);
    
    /* register input callbacks */
    glfwSetKeyCallback(glfw_window, key_callback);
    glfwSetCursorPosCallback(glfw_window, mouse_position_callback);
    glfwSetMouseButtonCallback(glfw_window, mouse_button_callback);
    glfwSetScrollCallback(glfw_window, scroll_callback);
    
    /* create window object */
    g_window.window = glfw_window;
    
    return 1;
}

void
window_deinit(void)
{
    if(g_window.window)
        glfwDestroyWindow(g_window.window);
}

uint32_t
window_width(void)
{
    return g_window.width;
}

uint32_t window_height(void)
{
    return g_window.height;
}

SERVICE(window_width_wrapper)
{
    SERVICE_RETURN(uint32_t, g_window.width);
}

SERVICE(window_height_wrapper)
{
    SERVICE_RETURN(uint32_t, g_window.height);
}

EVENT_LISTENER0(on_render)
{
    /* render everything */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw_2d();
    text_draw();
    glfwSwapBuffers(g_window.window);

    glfwPollEvents();

    /* fire close window event */
    if(glfwWindowShouldClose(g_window.window) != 0)
    {
        EVENT_FIRE0(evt_close_window);
    }
}
