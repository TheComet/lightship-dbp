#include <Python.h>
#include "structmember.h"

/* -------------------------------------------------------------------------
 * Game object
 * ------------------------------------------------------------------------- */

struct Game
{
	PyObject_HEAD
	PyObject* service;
};

/* ------------------------------------------------------------------------- */
static PyObject*
Game_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	struct Game* self;

	self = (struct Game*)type->tp_alloc(type, 0);

	/* self could be NULL */
	return (PyObject*)self;
}

/* ------------------------------------------------------------------------- */
static void
Game_dealloc(struct Game* self)
{
	Py_XDECREF(self->service);
	Py_TYPE(self)->tp_free((PyObject*)self);
}

/* ------------------------------------------------------------------------- */
static PyObject*
Game_test(PyObject* self, PyObject* args)
{
	puts("hello world");
	Py_INCREF(Py_None);
	return Py_None;
}

/* ------------------------------------------------------------------------- */
static PyMemberDef Game_members[] = {
	{"service", T_OBJECT_EX, offsetof(struct Game, service), 0, "service"},
	{NULL}
};

static PyMethodDef Game_methods[] = {
	{"test", Game_test, METH_VARARGS, "test method"},
	{NULL}
};

static PyTypeObject GameType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"lightship.game",          /* tp_name */
	sizeof(struct Game),       /* tp_basicsize */
	0,                         /* tp_itemsize */
    (destructor)Game_dealloc,  /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT |       /* tp_flags */
	    Py_TPFLAGS_BASETYPE,
	"Game objects",            /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	Game_methods,              /* tp_methods */
	Game_members,              /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	0,                         /* tp_init */
	0,                         /* tp_alloc */
	Game_new,                  /* tp_new */
};

/* -------------------------------------------------------------------------
 * Service object
 * ------------------------------------------------------------------------- */

struct Service
{
	PyObject_HEAD
	PyObject* register_;
};

/* ------------------------------------------------------------------------- */
static PyObject*
Service_init(PyObject* self, PyObject* args)
{
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject*
Service_register(PyObject* self, PyObject* args)
{
	Py_INCREF(Py_None);
	return Py_None;
}

/* -------------------------------------------------------------------------
 * lightship module
 * ------------------------------------------------------------------------- */

static PyMethodDef lightship_module_methods[] = {
	/*{"test", lightship_test, METH_VARARGS, "docstring"},*/
	{NULL}
};

static PyModuleDef lightship_module = {
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
	GameType.tp_new = PyType_GenericNew;
	if(PyType_Ready(&GameType) < 0)
		return 0;

	return 1;
}

static void
add_built_in_types_to_module(PyObject* module)
{
	Py_INCREF(&GameType);
	PyModule_AddObject(module, "Game", (PyObject*)&GameType);
}

static void
add_module_attributes(PyObject* module)
{
	PyObject* game_object = PyObject_CallObject((PyObject*)&GameType, NULL);
	PyObject_SetAttrString(module, "game", game_object);
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

	add_built_in_types_to_module(module);
	add_module_attributes(module);

	return module;
}
