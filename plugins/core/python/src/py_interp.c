#include "plugin_python/py_interp.h"
#include "plugin_python/config.h"
#include "plugin_python/glob.h"
#include "framework/log.h"
#include <unistd.h> /* getcwd */
#include <Python.h>

static uint32_t g_game_references = 0;

/* ------------------------------------------------------------------------- */
char
init_python(struct glob_t* g)
{
	char cwd[1024];

	/*
	 * If there is more than one game referencing this plugin, don't
	 * re-initialise the python interpreter. Just increment and return.
	 */
	if(g_game_references++)
		return 1;

	/* try to tell python to use the current working directory */
	if(getcwd(cwd, sizeof(cwd)) != NULL)
		Py_SetProgramName(cwd);
	else
		llog(LOG_WARNING, g->game, PLUGIN_NAME, "Couldn't set current working directory.");

	/* initialise interpreter */
	llog(LOG_INFO, g->game, PLUGIN_NAME, "Initialising python interpreter");
	Py_Initialize();

	return 1;
}

/* ------------------------------------------------------------------------- */
void deinit_python(struct glob_t* g)
{
	/*
	 * Only finalise if we are the last game using the python interpreter.
	 * Otherwise decrement reference count and return.
	 */
	if(!(--g_game_references))
	{
		llog(LOG_INFO, g->game, PLUGIN_NAME, "De-initialising python interpreter");
		Py_Finalize();
	}
}
