#include "util/config.h"
#include "util/plugin.h"
#include "util/log.h"
#include "plugin_renderer_gl/config.h"
#include "plugin_renderer_gl/window.h"
#include "plugin_renderer_gl/events.h"
#include "plugin_renderer_gl/services.h"
#include "plugin_renderer_gl/2d.h"
#include "plugin_renderer_gl/text.h"
#include "plugin_renderer_gl/text_manager.h"
#include "plugin_renderer_gl/text_wrapper.h"
#include "glfw3.h"
#include <stdio.h>

static struct plugin_t* g_plugin = NULL;

void
set_plugin_info(void)
{
    /* set plugin information */
    plugin_set_info(g_plugin,
            PLUGIN_NAME,            /* name */
            PLUGIN_CATEGORY,        /* category */
            PLUGIN_AUTHOR,          /* author */
            PLUGIN_DESCRIPTION,     /* description */
            PLUGIN_WEBSITE          /* website */
    );
    plugin_set_programming_language(g_plugin,
            PLUGIN_PROGRAMMING_LANGUAGE_C
    );
    plugin_set_version(g_plugin,
            PLUGIN_VERSION_MAJOR,
            PLUGIN_VERSION_MINOR,
            PLUGIN_VERSION_PATCH
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
        llog(LOG_ERROR, PLUGIN_NAME, 1, "Failed to initialise glfw");
        return PLUGIN_FAILURE;
    }

    if(!window_init())
        return PLUGIN_FAILURE;
    
    /* clear any GL errors caused by glfw and glew */
    glGetError();

    /* init graphics */
    init_2d();

    if(!text_manager_init())
        return PLUGIN_FAILURE;
    text_wrapper_init();

    register_event_listeners(g_plugin);

    return PLUGIN_SUCCESS;
}

PLUGIN_STOP()
{
    text_wrapper_deinit();
    text_manager_deinit();
    deinit_2d();
    window_deinit();
    glfwTerminate();
}

PLUGIN_DEINIT()
{
    plugin_destroy(g_plugin);
}