#include <Python.h>
#include "structmember.h"

/* -------------------------------------------------------------------------
 * Service object.
 * Callable object allowing python scripts to call internal services of
 * lightship.
 * -------------------------------------------------------------------------*/

struct Service
{
	PyObject_HEAD
};

static PyTypeObject ServiceType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"lightship.Service",           /* tp_name */
	sizeof(struct Service),        /* tp_basicsize */
	0,                             /* tp_itemsize */
    0,                             /* tp_dealloc */
	0,                             /* tp_print */
	0,                             /* tp_getattr */
	0,                             /* tp_setattr */
	0,                             /* tp_reserved */
	0,                             /* tp_repr */
	0,                             /* tp_as_number */
	0,                             /* tp_as_sequence */
	0,                             /* tp_as_mapping */
	0,                             /* tp_hash  */
	0,                             /* tp_call */
	0,                             /* tp_str */
	0,                             /* tp_getattro */
	0,                             /* tp_setattro */
	0,                             /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT |           /* tp_flags */
	    Py_TPFLAGS_BASETYPE,
	"Service objects",             /* tp_doc */
};

/* -------------------------------------------------------------------------
 * Services object
 * Has a static method for creating and destroying services.
 * Has Service instances as attributes which can be used to call existing
 * services.
 * ------------------------------------------------------------------------- */

struct Services
{
	PyObject_HEAD
	PyObject* create;
	PyObject* destroy;
};

/* ------------------------------------------------------------------------- */
static void
Services_dealloc(struct Services* self)
{
	Py_XDECREF(self->create);
	Py_XDECREF(self->destroy);
	Py_TYPE(self)->tp_free((PyObject*)self);
}

/* ------------------------------------------------------------------------- */
static PyObject*
Services_create(PyObject* self, PyObject* args)
{
	puts("Services_create()");
	Py_INCREF(Py_None);
	return Py_None;
}

/* ------------------------------------------------------------------------- */
static PyObject*
Services_destroy(PyObject* self, PyObject* args)
{
	Py_INCREF(Py_None);
	return Py_None;
}

/* ------------------------------------------------------------------------- */
static PyMethodDef Services_methods[] = {
	{"create", Services_create, METH_STATIC | METH_VARARGS, "creates and registers a new service."},
	{"destroy", Services_destroy, METH_VARARGS | METH_VARARGS, "destroys and unregisters an existing service."},
	{NULL}
};

static PyTypeObject ServicesType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"lightship.Services",          /* tp_name */
	sizeof(struct Services),       /* tp_basicsize */
	0,                             /* tp_itemsize */
    (destructor)Services_dealloc,  /* tp_dealloc */
	0,                             /* tp_print */
	0,                             /* tp_getattr */
	0,                             /* tp_setattr */
	0,                             /* tp_reserved */
	0,                             /* tp_repr */
	0,                             /* tp_as_number */
	0,                             /* tp_as_sequence */
	0,                             /* tp_as_mapping */
	0,                             /* tp_hash  */
	0,                             /* tp_call */
	0,                             /* tp_str */
	0,                             /* tp_getattro */
	0,                             /* tp_setattro */
	0,                             /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT |           /* tp_flags */
	    Py_TPFLAGS_BASETYPE,
	"Services objects",            /* tp_doc */
	0,                             /* tp_traverse */
	0,                             /* tp_clear */
	0,                             /* tp_richcompare */
	0,                             /* tp_weaklistoffset */
	0,                             /* tp_iter */
	0,                             /* tp_iternext */
	Services_methods               /* tp_methods */
};

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
	if(!self)
		return NULL;

	self->service = PyObject_CallObject((PyObject*)&ServicesType, NULL);
	if(!self->service)
	{
		Py_DECREF(self);
		return NULL;
	}

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
	"lightship.Game",          /* tp_name */
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
