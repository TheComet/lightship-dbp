#include <Python.h>
#include "plugin_python/config.h"
#include "plugin_python/py_interp.h"
#include "plugin_python/context.h"
#include "framework/log.h"
#include "util/string.h"
#include <unistd.h> /* getcwd */

static uint32_t g_game_references = 0;

PyMODINIT_FUNC
PyInit_lightship(void);

/* ------------------------------------------------------------------------- */
char
init_python(struct context_t* g)
{
	char cwd[1024];

	/*
	 * If there is more than one game referencing this plugin, don't
	 * re-initialise the python interpreter. Just increment and return.
	 */
	if(g_game_references++)
		return 1;

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
		llog(LOG_WARNING, g->game, PLUGIN_NAME, "Couldn't set current working directory.");

	/* initialise interpreter */
	llog(LOG_INFO, g->game, PLUGIN_NAME, "Initialising python interpreter");
	Py_Initialize();

	PyRun_SimpleString(
		"import lightship\n"
		"lightship.game.service.create()\n");

	return 1;
}

/* ------------------------------------------------------------------------- */
void deinit_python(struct context_t* g)
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
