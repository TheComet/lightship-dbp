#include "renderer_gl/config.h"
#include "util/config.h"
#include "util/plugin.h"
#include <stdio.h>

#include "GL/glew.h"
#include "glfw3.h"

struct plugin_t* g_plugin = NULL;
GLFWwindow* g_window = NULL;

LIGHTSHIP_PUBLIC_API struct plugin_t* plugin_init(void)
{
    g_plugin = plugin_create();
    
    /* set plugin information */
    plugin_set_info(g_plugin,
            "renderer_gl",                  /* name */
            "graphics",                     /* category */
            "TheComet",                     /* author */
            "OpenGL plugin for lightship",  /* description */
            "http://github.com/TheComet93/" /* website */
    );
    plugin_set_programming_language(g_plugin,
            PLUGIN_PROGRAMMING_LANGUAGE_C
    );
    plugin_set_version(g_plugin,
            RENDERER_GL_VERSION_MAJOR,
            RENDERER_GL_VERSION_MINOR,
            RENDERER_GL_VERSION_PATCH
    );

    return g_plugin;
}

LIGHTSHIP_PUBLIC_API plugin_result_t plugin_start(void)
{
    /* initialise GLFW */
    if(!glfwInit())
    {
        fprintf(stderr, "Error: failed to initialise glfw\n");
        return PLUGIN_FAILURE;
    }
    
    /* configure preferences */
    glfwWindowHint(GLFW_SAMPLES, 4); /* 4x antialiasing */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); /* want GL 3.3 */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); /* to make macOS happy */
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); /* don't want the old OpenGL */
    
    /* open window and create OpenGL context */
    g_window = glfwCreateWindow(800, 600, "Light Ship", NULL, NULL);
    if(g_window == NULL)
    {
        fprintf(stderr, "Error: Failed to open glfw window. If you have an Intel GPU, they are not 3.3 compatible.\n");
        glfwTerminate();
        return PLUGIN_FAILURE;
    }
    
    return PLUGIN_SUCCESS;
}

LIGHTSHIP_PUBLIC_API void plugin_stop(void)
{
    glfwTerminate();
    plugin_destroy(g_plugin);
}
