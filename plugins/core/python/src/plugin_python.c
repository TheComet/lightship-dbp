#include "plugin_python/config.h"   /* configurations for this plugin */
#include "plugin_python/context.h"
#include "plugin_python/services.h" /* plugin services */
#include "plugin_python/events.h"   /* plugin events */
#include "plugin_python/py_interp.h"
#include "framework/plugin.h"	    /* plugin api */

/* ------------------------------------------------------------------------- */
PLUGIN_PYTHON_PUBLIC_API PLUGIN_INIT()
{
	struct plugin_t* plugin;
	struct context_t* context;

	/* init context store */
	context_create(game);
	context = get_context(game);
	unordered_vector_init_vector(&context->py_objs.games, sizeof(void*));

	/* create plugin object - host requires this */
	/* plugin information can be changed in the file "CMakeLists.txt" */
	plugin = plugin_create(game,
			PLUGIN_NAME,
			PLUGIN_CATEGORY,
			PLUGIN_AUTHOR,
			PLUGIN_DESCRIPTION,
			PLUGIN_WEBSITE);

	/* add the plugin to the context struct for later access */
	get_context(game)->plugin = plugin;

	/* set other info */
	plugin_set_programming_language(plugin,
			PLUGIN_PROGRAMMING_LANGUAGE_C);
	plugin_set_version(plugin,
			PLUGIN_VERSION_MAJOR,
			PLUGIN_VERSION_MINOR,
			PLUGIN_VERSION_PATCH);

	register_services(plugin);
	register_events(plugin);

	return plugin;
}

/* ------------------------------------------------------------------------- */
PLUGIN_PYTHON_PUBLIC_API PLUGIN_START()
{
	struct context_t* context = get_context(game);

	/* hook in to services and events */
	if(!get_required_services(context->plugin))
		return PLUGIN_FAILURE;
	get_optional_services(context->plugin);
	register_event_listeners(context->plugin);

	if(!init_python(context))
		return PLUGIN_FAILURE;

	return PLUGIN_SUCCESS;
}

/* ------------------------------------------------------------------------- */
PLUGIN_PYTHON_PUBLIC_API PLUGIN_STOP()
{
	struct context_t* context = get_context(game);

	deinit_python(context);
}

/* ------------------------------------------------------------------------- */
PLUGIN_PYTHON_PUBLIC_API PLUGIN_DEINIT()
{
	struct context_t* context = get_context(game);

	plugin_destroy(context->plugin);
	unordered_vector_clear_free(&context->py_objs.games);
	context_destroy(game);
}
