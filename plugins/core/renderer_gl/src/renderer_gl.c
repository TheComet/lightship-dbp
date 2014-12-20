#include "lightship/api.h" /* lightship API so we can register and call services */
#include "util/config.h"
#include "util/plugin.h"
#include "renderer_gl/config.h"
#include "renderer_gl/window.h"

#include "GL/glew.h"
#include "glfw3.h"

#include <stdio.h>

struct plugin_t* g_plugin = NULL;
struct window_t* g_window = NULL;

void hello(void)
{
    printf("hello world\n");
}

LIGHTSHIP_PUBLIC_API struct plugin_t* plugin_init(struct lightship_api_t* api)
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

    api->service_register(g_plugin, "hello", (intptr_t)hello);

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

    g_window = window_create();

    return PLUGIN_SUCCESS;
}

LIGHTSHIP_PUBLIC_API void plugin_stop(void)
{
    if(g_window)
        window_destroy(g_window);

    glfwTerminate();
    plugin_destroy(g_plugin);
}
