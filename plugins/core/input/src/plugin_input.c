#include "framework/plugin.h"     /* plugin api */
#include "plugin_input/config.h"   /* configurations for this plugin */
#include "plugin_input/services.h" /* plugin services */
#include "plugin_input/events.h"   /* plugin events */
#include "plugin_input/glob.h"

/* ------------------------------------------------------------------------- */
PLUGIN_INPUT_PUBLIC_API PLUGIN_INIT()
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

	/* set plugin information - Change this in the file "CMakeLists.txt" */
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
PLUGIN_INPUT_PUBLIC_API PLUGIN_START()
{
	if(!get_required_services())
		return PLUGIN_FAILURE;
	get_optional_services();
	register_event_listeners(get_global(game)->plugin);

	return PLUGIN_SUCCESS;
}

/* ------------------------------------------------------------------------- */
PLUGIN_INPUT_PUBLIC_API PLUGIN_STOP()
{
}

/* ------------------------------------------------------------------------- */
PLUGIN_INPUT_PUBLIC_API PLUGIN_DEINIT()
{
	plugin_destroy(get_global(game)->plugin);
	glob_destroy(game);
}
