#include "plugin_menu/config.h"   /* configurations for this plugin */
#include "framework/plugin.h"     /* plugin api */
#include "plugin_menu/services.h" /* plugin services */
#include "plugin_menu/events.h"   /* plugin events */
#include "plugin_menu/context.h"
#include "plugin_menu/button.h"
#include "plugin_menu/menu.h"

/* ------------------------------------------------------------------------- */
PLUGIN_MENU_PUBLIC_API PLUGIN_INIT()
{
	struct plugin_t* plugin;

	/* init global data */
	context_create(game);

	/* init plugin */
	plugin = plugin_create(game,
						   PLUGIN_NAME,
						   PLUGIN_CATEGORY,
						   PLUGIN_AUTHOR,
						   PLUGIN_DESCRIPTION,
						   PLUGIN_WEBSITE
	);
	get_context(game)->plugin = plugin;

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
PLUGIN_MENU_PUBLIC_API PLUGIN_START()
{
	struct context_t* context = get_context(game);

	if(!get_required_services(context->plugin))
		return PLUGIN_FAILURE;
	get_optional_services(context->plugin);
	register_event_listeners(context->plugin);

	element_init(context);
	button_init(context);
	menu_init(context);

	return PLUGIN_SUCCESS;
}

/* ------------------------------------------------------------------------- */
PLUGIN_MENU_PUBLIC_API PLUGIN_STOP()
{
	struct context_t* context;

	/* de-init */
	context = get_context(game);
	menu_deinit(context);
	button_deinit(context);
}

/* ------------------------------------------------------------------------- */
PLUGIN_MENU_PUBLIC_API PLUGIN_DEINIT()
{
	plugin_destroy(get_context(game)->plugin);
	context_destroy(game);
}
