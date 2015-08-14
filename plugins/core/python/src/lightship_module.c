#include "plugin_python/lightship_module.h"
#include "plugin_python/lightship_module_game.h"
#include "plugin_python/lightship_module_events.h"
#include "plugin_python/lightship_module_services.h"

#include <structmember.h>

#include "plugin_python/config.h"
#include "plugin_python/context.h"
#include "framework/game.h"
#include "framework/log.h"
#include "util/unordered_vector.h"

/* ------------------------------------------------------------------------- */
static PyObject*
lightship_module_register_game(PyObject* self, PyObject* game)
{
	struct unordered_vector_t* games_vec;

	/* make sure game object is actually an instance of GameType */
	if(!PyObject_IsInstance(game, (PyObject*)&GameType))
	{
		PyErr_SetString(lightship_error,
						"Object is not a subclass of lightship.Game");
		return NULL;
	}

	/* add game object to context store so it can be accessed later */
	games_vec = &(get_context(g_injected_game)->py_objs.games);
	Py_INCREF(game);
	unordered_vector_push(games_vec, &game);

	Py_INCREF(Py_None);
	return Py_None;
}

/* ------------------------------------------------------------------------- */
static PyObject*
lightship_module_unregister_game(PyObject* self, PyObject* game)
{
	struct unordered_vector_t* games_vec;

	/* make sure game object is actually an instance of GameType */
	if(!PyObject_IsInstance(game, (PyObject*)&GameType))
	{
		PyErr_SetString(lightship_error,
						"Object is not a subclass of lightship.Game");
		return NULL;
	}

	/* remove game from context store */
	games_vec = &(get_context(((struct Game*)game)->game)->py_objs.games);
	UNORDERED_VECTOR_FOR_EACH(games_vec, PyObject*, p_game)
		if(*p_game == game)
		{
			unordered_vector_erase_element(games_vec, p_game);
			Py_DECREF(game);
			break;
		}
	UNORDERED_VECTOR_END_EACH

	Py_INCREF(Py_None);
	return Py_None;
}

/* ------------------------------------------------------------------------- */
static PyMethodDef lightship_module_methods[] = {
	{"register_game", lightship_module_register_game, METH_O, "registers a new game object."},
	{"unregister_game", lightship_module_unregister_game, METH_O, "unregisters an existing game object"},
	{NULL}
};

PyModuleDef lightship_module = {
	PyModuleDef_HEAD_INIT,
	"lightship",              /* module name */
	"",                       /* docstring, may be NULL */
	-1,                       /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	lightship_module_methods, /* module methods */
	NULL,                     /* m_reload */
	NULL,                     /* m_traverse */
	NULL,                     /* m_clear */
	NULL                      /* m_free */
};

static char
init_built_in_types()
{
	if(PyType_Ready(&GameType) < 0)
		return 0;

	ServicesType.tp_new = PyType_GenericNew;
	if(PyType_Ready(&ServicesType) < 0)
		return 0;

	ServiceType.tp_new = PyType_GenericNew;
	if(PyType_Ready(&ServiceType) < 0)
		return 0;

	return 1;
}

static void
add_built_in_types_to_module(PyObject* module)
{
	Py_INCREF(&GameType);
	PyModule_AddObject(module, "Game", (PyObject*)&GameType);

	Py_INCREF(&ServicesType);
	PyModule_AddObject(module, "Services", (PyObject*)&ServicesType);

	Py_INCREF(&ServiceType);
	PyModule_AddObject(module, "Service", (PyObject*)&ServiceType);
}

PyMODINIT_FUNC
PyInit_lightship(void)
{
	PyObject* module;

	if(!init_built_in_types())
		return NULL;

	/* create the module */
	if(!(module = PyModule_Create(&lightship_module)))
		return NULL;

	create_lightship_exceptions(module);
	add_built_in_types_to_module(module);

	return module;
}
