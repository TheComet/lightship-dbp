#include "lightship/api.h" /* lightship API so we can register and call services */
#include "util/config.h"
#include "util/plugin.h"
#include "renderer_gl/config.h"
#include "renderer_gl/window.h"
#include "renderer_gl/events.h"
#include "renderer_gl/services.h"

#include "GL/glew.h"
#include "glfw3.h"

#include <stdio.h>

struct plugin_t* g_plugin = NULL;

PLUGIN_INIT()
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
    
    register_services(g_plugin, api);
    register_events(g_plugin, api);
    
    window_init();

    return g_plugin;
}

PLUGIN_START()
{
    /* initialise GLFW */
    if(!glfwInit())
    {
        fprintf(stderr, "Error: failed to initialise glfw\n");
        return PLUGIN_FAILURE;
    }

    if(!window_init())
        return PLUGIN_FAILURE;
    
    register_event_listeners(g_plugin, api);

    return PLUGIN_SUCCESS;
}

PLUGIN_STOP()
{
    window_deinit();

    glfwTerminate();
    plugin_destroy(g_plugin);
}
