#include "renderer_gl/window.h"
#include "renderer_gl/events.h"
#include "util/linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GL/glew.h"
#include "glfw3.h"

struct window_t g_window;

char window_init(void)
{
    GLFWwindow* glfw_window;
    
    memset(&g_window, 0, sizeof(struct window_t));

    /* configure preferences */
    glfwWindowHint(GLFW_SAMPLES, 4); /* 4x antialiasing */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); /* want GL 3.3 */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); /* to make macOS happy */
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); /* don't want the old OpenGL */
    
    /* window dimensions */
    g_window.width = 800;
    g_window.height = 600;
    
    /* open window and create OpenGL context */
    glfw_window = glfwCreateWindow(g_window.width, g_window.height, "Light Ship", NULL, NULL);
    if(glfw_window == NULL)
    {
        fprintf(stderr, "Error: Failed to open glfw window. If you have an Intel GPU, they are not 3.3 compatible.\n");
        return 0;
    }
    glfwMakeContextCurrent(glfw_window); /* initialise GLEW */
    glewExperimental = 1; /* needed in core profile */
    if(glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialise GLEW\n");
        return 0;
    }
    
    /* ensure the escape key can be captured */
    glfwSetInputMode(glfw_window, GLFW_STICKY_KEYS, GL_TRUE);
    
    /* create window object */
    g_window.window = glfw_window;
    
    return 1;
}

void window_deinit(void)
{
    if(g_window.window)
        glfwDestroyWindow(g_window.window);
}

EVENT_LISTENER(on_render, void* arg)
{
    /* render everything */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(g_window.window);

    glfwPollEvents();

    /* fire close window event */
    if(glfwWindowShouldClose(g_window.window) != 0)
    {
        EVENT_FIRE(evt_close_window, NULL)
    }
}
