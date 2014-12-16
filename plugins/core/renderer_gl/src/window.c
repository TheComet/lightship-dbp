#include "renderer_gl/window.h"
#include "glfw3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct window_t* window_create(void)
{
    GLFWwindow* glfw_window;
    struct window_t* window;

    /* configure preferences */
    glfwWindowHint(GLFW_SAMPLES, 4); /* 4x antialiasing */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); /* want GL 3.3 */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); /* to make macOS happy */
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); /* don't want the old OpenGL */
    
    /* open window and create OpenGL context */
    glfw_window = glfwCreateWindow(800, 600, "Light Ship", NULL, NULL);
    if(glfw_window == NULL)
    {
        fprintf(stderr, "Error: Failed to open glfw window. If you have an Intel GPU, they are not 3.3 compatible.\n");
        return NULL;
    }
    
    /* create window object */
    window = (struct window_t*)malloc(sizeof(struct window_t));
    memset(window, 0, sizeof(struct window_t));
    window->window = glfw_window;
    
    return window;
}

void window_destroy(struct window_t* window)
{
    glfwDestroyWindow(window->window);
    free(window);
}