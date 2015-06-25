#include "plugin_renderer_gl/config.h"
#include "plugin_renderer_gl/events.h"
#include "plugin_renderer_gl/services.h"
#include "plugin_renderer_gl/glob.h"
#include "plugin_renderer_gl/2d.h"
#include "plugin_renderer_gl/sprite.h"
#include "plugin_renderer_gl/text_manager.h"
#include "plugin_renderer_gl/text_wrapper.h"
#include "plugin_renderer_gl/window.h"
#include "framework/game.h"
#include "framework/plugin.h"
#include "framework/log.h"
#include "glfw3.h"
#include <stdio.h>

/* ------------------------------------------------------------------------- */
PLUGIN_RENDERER_GL_PUBLIC_API PLUGIN_INIT()
{
	struct plugin_t* plugin;

	/* init global data */
	glob_create(game);

	/* init plugin */
	plugin = plugin_create(game,
						   PLUGIN_NAME,
						   PLUGIN_CATEGORY,
						   PLUGIN_AUTHOR,
						   PLUGIN_DESCRIPTION,
						   PLUGIN_WEBSITE
	);
	get_global(game)->plugin = plugin;

	/* set plugin information */
	plugin_set_programming_language(plugin,
			PLUGIN_PROGRAMMING_LANGUAGE_C
	);
	plugin_set_version(plugin,
			PLUGIN_VERSION_MAJOR,
			PLUGIN_VERSION_MINOR,
			PLUGIN_VERSION_PATCH
	);

	register_services(plugin);
	register_events(plugin);

	return plugin;
}

/* ------------------------------------------------------------------------- */
PLUGIN_RENDERER_GL_PUBLIC_API PLUGIN_START()
{
	struct glob_t* g;

	/* get global struct */
	g = get_global(game);

	/* initialise GLFW */
	if(!glfwInit())
	{
		llog(LOG_ERROR, game, PLUGIN_NAME, "Failed to initialise glfw");
		return PLUGIN_FAILURE;
	}

	/* creates the window */
	if(!window_init(g))
		return PLUGIN_FAILURE;

	/* clear any GL errors caused by glfw and glew */
	glGetError();

	/* init graphics components */
	if(!init_2d(g))
		return PLUGIN_FAILURE;
	if(!text_manager_init(g))
		return PLUGIN_FAILURE;
	if(!text_wrapper_init())
		return PLUGIN_FAILURE;
	if(!sprite_init(g))
		return PLUGIN_FAILURE;

	register_event_listeners(g->plugin);

	{
		uint32_t id;
		struct sprite_t* sprite = sprite_create(g, "menu/join/join.png", 1, 1, 1, &id);
		/*sprite_scale(sprite, 0.3);
		sprite_set_position(sprite, 0.2, 0.7);*/
	}

	return PLUGIN_SUCCESS;
}

/* ------------------------------------------------------------------------- */
PLUGIN_RENDERER_GL_PUBLIC_API PLUGIN_STOP()
{
	sprite_deinit();
	text_wrapper_deinit();
	text_manager_deinit();
	deinit_2d();
	window_deinit();
	glfwTerminate();
}

/* ------------------------------------------------------------------------- */
PLUGIN_RENDERER_GL_PUBLIC_API PLUGIN_DEINIT()
{
	plugin_destroy(get_global(game)->plugin);
	glob_destroy(game);
}
