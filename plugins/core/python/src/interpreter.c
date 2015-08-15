#include "plugin_python/lightship_module.h"
#include "plugin_python/lightship_module_game.h"
#include "plugin_python/config.h"
#include "plugin_python/interpreter.h"
#include "plugin_python/context.h"
#include "framework/log.h"
#include "util/string.h"
#include "util/yaml.h"
#include <unistd.h> /* getcwd */

static uint32_t g_game_references = 0;

PyMODINIT_FUNC
PyInit_lightship(void);

/* ------------------------------------------------------------------------- */
char
init_python(struct context_t* context)
{
	/*
	 * If there is more than one game referencing this plugin, don't
	 * re-initialise the python interpreter.
	 */
	if(!g_game_references++)
	{
		char cwd[1024];

		/* add built in modules */
		PyImport_AppendInittab("lightship", PyInit_lightship);

		/* try to tell python to use the current working directory */
		if(getcwd(cwd, sizeof(cwd)) != NULL)
		{
			wchar_t* cwd_w = strtowcs(cwd);
			Py_SetProgramName(cwd_w);
			free_string(cwd_w);
		}
		else
			llog(LOG_WARNING, context->game, PLUGIN_NAME, "Couldn't set current working directory.");

		/* initialise interpreter */
		llog(LOG_INFO, context->game, PLUGIN_NAME, "Initialising python interpreter");
		Py_Initialize();
	}

	for(;;)
	{
		const char* file_name;
		FILE* fd;

		if(!context->game->settings)
		{
			llog(LOG_WARNING, context->game, PLUGIN_NAME, "Game has no settings, not loading any python scripts");
			break;
		}
		;
		if(!(file_name  = yaml_get_value(context->game->settings, "python.main")))
		{
			llog(LOG_WARNING, context->game, PLUGIN_NAME, "Key \"python.main\" wasn't found in game settings. No python scripts will be executed.");
			break;
		}

		if(!(fd = fopen(file_name, "r")))
		{
			llog(LOG_ERROR, context->game, PLUGIN_NAME, "Failed to open python script \"%s\"", file_name);
			break;
		}

		g_injected_context = context;
		PyRun_SimpleFile(fd, file_name);
		g_injected_context = NULL;

		fclose(fd);
		break;
	}
	return 1;

}

/* ------------------------------------------------------------------------- */
void deinit_python(struct context_t* context)
{
	/*
	 * Only finalise if we are the last game using the python interpreter.
	 * Otherwise decrement reference count and return.
	 */
	if(!(--g_game_references))
	{
		llog(LOG_INFO, context->game, PLUGIN_NAME, "De-initialising python interpreter");
		Py_Finalize();
	}
}
