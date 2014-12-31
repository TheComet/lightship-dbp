#include "lightship/api.h" /* lightship API so we can register and call services */
#include "util/config.h"
#include "util/plugin.h"
#include "plugin_renderer_gl/config.h"
#include "plugin_renderer_gl/window.h"
#include "plugin_renderer_gl/events.h"
#include "plugin_renderer_gl/services.h"
#include "plugin_renderer_gl/2d.h"

#include "glfw3.h"

#include <stdio.h>

struct plugin_t* g_plugin = NULL;

void
set_plugin_info(void)
{
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
}

PLUGIN_INIT()
{
    g_plugin = plugin_create();
    set_plugin_info();
    register_services(g_plugin);
    register_events(g_plugin);

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
    
    /* init graphics */
    init_2d();
    batch_2d_begin();
        line(-0.5, -1, 1, 1, 0xFF0000FF, 0x00FF00FF);
    batch_2d_end();
    
    register_event_listeners(g_plugin);

    return PLUGIN_SUCCESS;
}

PLUGIN_STOP()
{
    deinit_2d();
    window_deinit();

    glfwTerminate();
    plugin_destroy(g_plugin);
}
