#include "framework/services.h"
#include "framework/plugin.h"
#include "plugin_python/services.h"
#include "plugin_python/glob.h"
#include <string.h>

/* ------------------------------------------------------------------------- */
void
register_services(struct plugin_t* plugin)
{
	/* ------------------------------------------------------------------------
	 * All services this plugin exposes.
	 * --------------------------------------------------------------------- */

	/*struct service_t* s;*/
	/*
	 * SERVICE_REGISTER0(plugin, s, PLUGIN_NAME ".service_name_1, service_callback_1, return_type);
	 * SERVICE_REGISTER2(plugin, s, PLUGIN_NAME ".service_name_2, service_callback_2, return_type, argument_type_1, argument_type_2);
	 * etc...
	 */

}

/* ------------------------------------------------------------------------- */
char
get_required_services(struct plugin_t* plugin)
{
	/* get service glob and set every service pointer to NULL */
	/*
	 * See glob.h. All service objects cannot be stored statically. Instead, they
	 * are inserted into a glob struct, which allows each plugin to be loaded
	 * multiple times without static data ever conflicting. Every plugin owns
	 * one glob struct which can be retrieved from the game object.
	 *
	 * The service objects you want to register can be added to the glob struct
	 * in glob.h.
	 *
	struct glob_services_t* g_services = &get_global(game)->services;
	memset(g_services, 0, sizeof *g_services);*/

	/*
	 * if(!(g->name_1 = service_get(game, "plugin.service1")))
	 *	 return 0;
	 * if(!(g->name_2 = service_get(game, "plugin.service2")))
	 *	 return 0;
	 * etc...
	 */

	return 1;
}

/* ------------------------------------------------------------------------- */
void
get_optional_services(struct plugin_t* plugin)
{
}
