#include <renderer_gl/config.h>
#include <util/config.h>
#include <util/plugin.h>
#include <stdio.h>

struct plugin_t* g_plugin;

LIGHTSHIP_PUBLIC_API struct plugin_t* plugin_init(void)
{
    g_plugin = plugin_create();
    
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
    printf("hello world! I am a graphics plugin.\n");
    return PLUGIN_SUCCESS;
}

LIGHTSHIP_PUBLIC_API void plugin_stop(void)
{
    plugin_destroy(g_plugin);
}
