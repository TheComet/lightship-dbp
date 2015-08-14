#include "framework/plugin_manager.h"
#include "framework/events.h"
#include "framework/services.h"
#include "framework/game.h"
#include "framework/log.h"
#include "util/memory.h"
#include "util/yaml.h"

struct ptree_t;

#ifdef _DEBUG
static const char* yml_settings = "../../lightship/cfg/settings_d.yml";
#else
static const char* yml_settings = "cfg/settings.yml";
#endif

/* -------------------------------------------------------------------------- */
char
load_core_plugins(struct game_t* game)
{
	struct ptree_t* plugins_node;

	/*
		* Try to load and start the core plugins. If that fails, bail out.
		*/
	if(!game->settings)
	{
		llog(LOG_WARNING, game, NULL, "No core plugins will be loaded");
		return 1;
	}

	plugins_node = yaml_get_node(game->settings, "plugins");
	if(!plugins_node)
	{
		llog(LOG_WARNING, game, NULL, "Config file \"%s\" doesn't contain any plugins to load", yml_settings);
		return 0;
	}

	if(!load_plugins_from_yaml(game, plugins_node))
	{
		llog(LOG_FATAL, game, NULL, "Couldn't start all core plugins");
		return 0;
	}

	return 1;
}

/* -------------------------------------------------------------------------- */
char
init()
{
	/*
	 * Initialise memory management first.
	 */
	memory_init();

	/*
	 * Initialise global stuff.
	 */
	yaml_init();
	game_init();

	return 1;
}

/* -------------------------------------------------------------------------- */
char
init_game(char is_server)
{
	struct game_t *client, *localhost;

	/*
	 * Create the local game server. This is the context that holds all
	 * plugins, services, and events together.
	 */
	localhost = game_create("localhost", yml_settings, GAME_HOST);
	if(!localhost)
	{
		llog(LOG_FATAL, NULL, NULL, "Failed to create game");
		return 0;
	}

	/*
	 * Load and start the core plugins specified in the settings YAML file.
	 */
	if(!load_core_plugins(localhost))
	{
		game_destroy(localhost);
		return 0;
	}

	/*
	 * If we are a client, create client instance and connect to local server.
	 */
	if(!is_server)
	{
		client = game_create("localclient", yml_settings, GAME_CLIENT);
		if(!client)
			return 0;

		/* TODO: Remove - for now, to load client plugins */
		if(!load_core_plugins(client))
		{
			game_destroy(client);
			game_destroy(localhost);
			return 0;
		}

		game_connect(client, "localhost");
	}

	/* TODO: remove - for now, to bootstrap the menu */
	{
		char* menu_file_name;
		struct menu_t;
		struct menu_t* menu;
#ifdef _DEBUG
		menu_file_name = "../../plugins/core/menu/cfg/menu.yml";
#else
		menu_file_name = "cfg/menu.yml";
#endif
		SERVICE_CALL_NAME1(client, "menu.load", &menu, PTR(menu_file_name));
	}

	return 1;
}

/* -------------------------------------------------------------------------- */
void
deinit(void)
{
	/*
	 * De-init global stuff
	 */
	game_deinit();
	yaml_deinit();

	/*
	 * De-init memory management last
	 */
	memory_deinit();
}
