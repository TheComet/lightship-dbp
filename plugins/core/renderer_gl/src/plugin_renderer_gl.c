#include "lightship/api.h" /* lightship API so we can register and call services */
#include "util/config.h"
#include "util/plugin.h"
#include "util/log.h"
#include "plugin_renderer_gl/config.h"
#include "plugin_renderer_gl/window.h"
#include "plugin_renderer_gl/events.h"
#include "plugin_renderer_gl/services.h"
#include "plugin_renderer_gl/2d.h"
#include "plugin_renderer_gl/text.h"
#include "glfw3.h"
#include <stdio.h>

static struct plugin_t* g_plugin = NULL;
static struct font_t* g_font = NULL;
static const wchar_t* g_default_characters =
L"abcdefghijklmnopqrstuvwxyz"
L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
L"1234567890"
L" +-*/!?'^\"$%&()[]{}#@~,.";

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
#include "util/memory.h"
PLUGIN_START()
{
    /* initialise GLFW */
    if(!glfwInit())
    {
        llog(LOG_ERROR, 1, "Failed to initialise glfw");
        return PLUGIN_FAILURE;
    }

    if(!window_init())
        return PLUGIN_FAILURE;
    
    /* clear any GL errors caused by glfw and glew */
    glGetError();

    /* init graphics */
    init_2d();

    if(!text_init())
        return PLUGIN_FAILURE;
    if(!(g_font = text_load_font("ttf/DejaVuSans.ttf")))
        return PLUGIN_FAILURE;

    text_load_characters(g_font, g_default_characters);
    text_add_static_string(g_font, 0.0, 0.0, L"Hello World, testing text");

    register_event_listeners(g_plugin);

    return PLUGIN_SUCCESS;
}

PLUGIN_STOP()
{
    text_deinit();
    deinit_2d();
    window_deinit();

    glfwTerminate();
    plugin_destroy(g_plugin);
}
