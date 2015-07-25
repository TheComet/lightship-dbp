#include "framework/game.h"
#include "framework/plugin.h"
#include "framework/services.h"
#include "util/hash.h"
#include "framework/log.h"
#include "util/memory.h"
#include "util/string.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <wchar.h>

static void
service_free(struct service_t* service);

/* ------------------------------------------------------------------------- */
char
service_init(struct game_t* game)
{
	assert(game);

	ptree_init_ptree(&game->services, NULL);

	/* ------------------------------------------------------------------------
	 * Register built-in services
	 * --------------------------------------------------------------------- */

	for(;;)
	{
		if(!(game->service.start = service_create(game->core, "start", game_start_wrapper,
			dynamic_call_create_type_info("void", 0, NULL)))) break;
		if(!(game->service.pause = service_create(game->core, "pause", game_pause_wrapper,
			dynamic_call_create_type_info("void", 0, NULL)))) break;
		if(!(game->service.exit  = service_create(game->core, "exit",  game_exit_wrapper,
			dynamic_call_create_type_info("void", 0, NULL)))) break;

		return 1;
	}

	return 0;
}

/* ------------------------------------------------------------------------- */
void
service_deinit(struct game_t* game)
{
	ptree_destroy_keep_root(&game->services);
}

/* ------------------------------------------------------------------------- */
struct service_t*
service_create(struct plugin_t* plugin,
			   const char* directory,
			   const service_func exec,
			   struct type_info_t* type_info)
{
	struct service_t* service;
	struct ptree_t* node;

	assert(plugin);
	assert(plugin->game);
	assert(directory);
	assert(exec);
	assert(type_info);

	/* allocate and initialise service object */
	if(!(service = (struct service_t*)MALLOC(sizeof(struct service_t))))
		OUT_OF_MEMORY("service_create()", NULL);
	memset(service, 0, sizeof(struct service_t));

	/* if anything fails, break */
	for(;;)
	{
		service->plugin = plugin;
		service->exec = exec;
		service->type_info = type_info;

		/* plugin object keeps track of all created services */
		if(!unordered_vector_push(&plugin->services, &service))
			break;

		/* copy directory */
		if(!(service->directory = malloc_string(directory)))
			break;

		/* create node in game's service directory - want to do this last
		 * because ptree_remove_node uses malloc() */
		if(!(node = ptree_set(&plugin->game->services, directory, service)))
			break;

		/* NOTE: don't MALLOC() past this point ----------------------- */

		/* set the node's free function to service_free() to make deleting
		 * nodes easier */
		ptree_set_free_func(node, (ptree_free_func)service_free);

		/* success! */
		return service;
	}

	/* something went wrong, clean up everything */

	/* remove from plugin's list of services */
	unordered_vector_erase_element(&plugin->services, &service);

	if(service->directory)
		free_string(service->directory);

	return NULL;
}

/* ------------------------------------------------------------------------- */
void
service_destroy(struct service_t* service)
{
	struct ptree_t* node;
	struct game_t* game;
	char found_in_plugin;

	assert(service);
	assert(service->plugin);
	assert(service->plugin->game);
	assert(service->directory);

	/* get the node in the directory and verify */
	game = service->plugin->game;
	if(!(node = ptree_get_node(&game->services, service->directory)))
	{
		llog(LOG_ERROR, game, NULL, "Attempted to destroy the service \"%s\", "
			"but the associated game object with name \"%s\" doesn't own it! "
			"The service will not be destroyed.",
			 service->directory, game->name);
		return;
	}

	/* remove service from plugin */
	found_in_plugin = 0;
	UNORDERED_VECTOR_FOR_EACH(&service->plugin->services, struct service_t*, servp)
		if(*servp == service)
		{
			unordered_vector_erase_element(&service->plugin->services, servp);
			found_in_plugin = 1;
			break;
		}
	UNORDERED_VECTOR_END_EACH

	if(!found_in_plugin)
	{
		llog(LOG_ERROR, game, NULL, "Attempting to destroy the service \"%s\","
			" but the associated plugin object with name \"%s\" doesn't own "
			"it! The service will not be destroyed.",
			 service->directory, service->plugin->info.name);
		return;
	}

	/* destroying the node will free the service using ptree's free function */
	ptree_destroy(node);
}

/* ------------------------------------------------------------------------- */
static void
service_free(struct service_t* service)
{
	assert(service);
	assert(service->directory);
	assert(service->type_info);

	free_string(service->directory);
	dynamic_call_destroy_type_info(service->type_info);
	FREE(service);
}

/* ------------------------------------------------------------------------- */
uint32_t
service_destroy_all_matching(const char* pattern)
{
	return 0; /* TODO implement */
}

/* ------------------------------------------------------------------------- */
struct service_t*
service_get(struct game_t* game, const char* directory)
{
	struct ptree_t* node;

	assert(game);
	assert(directory);

	if(!(node = ptree_get_node(&game->services, directory)))
		return NULL;
	assert(node->value);
	return (struct service_t*)node->value;
}
