#include "plugin_python/lightship_module_services.h"
#include "plugin_python/lightship_module_exceptions.h"
#include "plugin_python/lightship_module_game.h"
#include "plugin_python/context.h"
#include "framework/services.h"
#include "util/memory.h"
#include "util/dynamic_call.h"
#include "util/string.h"
#include "util/macros.h"

/* -------------------------------------------------------------------------
 * Service object.
 * Callable object allowing python scripts to call internal services of
 * lightship.
 * -------------------------------------------------------------------------*/

struct Service
{
	PyObject_HEAD
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
PyTypeObject ServiceType = {
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
#pragma GCC diagnostic pop

/* -------------------------------------------------------------------------
 * Services object
 * Has static methods for creating and destroying services.
 * Has Service instances as attributes which can be used to call existing
 * services.
 * ------------------------------------------------------------------------- */

struct Services
{
	PyObject_HEAD
	PyObject* game;
	PyObject* create;
	PyObject* destroy;
};

/* ------------------------------------------------------------------------- */
static int
Services_init(struct Services* self, PyObject* game, PyObject* kwargs)
{
	PyObject* tmp;

	/* make sure game object is actually an instance of GameType */
	if(PyObject_IsInstance(game, (PyObject*)&GameType) == -1)
	{
		PyErr_SetString(lightship_error,
						"Object is not a subclass of lightship.Game");
		return -1;
	}

	/* inject game into services */
	tmp = self->game;
	Py_INCREF(game);
	self->game = game;
	Py_XDECREF(tmp);

	return 0;
}

/* ------------------------------------------------------------------------- */
static void
Services_dealloc(struct Services* self)
{
	Py_XDECREF(self->game);
	Py_XDECREF(self->create);
	Py_XDECREF(self->destroy);
	Py_TYPE(self)->tp_free((PyObject*)self);
}

/* ------------------------------------------------------------------------- */
char*
convert_python_type_to_lightship_type(PyObject* obj)
{
	if(Py_None == obj)
	{
		return malloc_string("void");
	} else if(PyLong_Check(obj))
	{
		return malloc_string("int32");
	} else if(PyUnicode_Check(obj))
	{
		return malloc_string("const char*");
	} else if(PyFloat_Check(obj))
	{
		return malloc_string("double");
	} else
	{
		return malloc_string("PYTHON_UNKNOWN_TYPE");
	}
}

static struct type_info_t*
create_type_info_for_python_callback(PyObject* py_ret_type, PyObject* py_arg_tuple)
{
	int argc, i;
	char* ret_type;
	char** argv;
	struct type_info_t* type_info;

	/* get number of arguments and allocate string array */
	argc = PySequence_Size(py_arg_tuple);
	argv = (char**)MALLOC(argc * sizeof(char*));

	/* convert each python object type to a string representation */
	ret_type = convert_python_type_to_lightship_type(py_ret_type);
	for(i = 0; i != argc; ++i)
	{
		PyObject* item = PySequence_Fast_GET_ITEM(py_arg_tuple, i);
		argv[i] = convert_python_type_to_lightship_type(item);
	}

	/* convert string representations to our internal type info struct */
	type_info = dynamic_call_create_type_info(ret_type, argc,
											  (const char**)argv);

	/* clean up */
	for(i = 0; i != argc; ++i)
		free_string(argv[i]);
	FREE(argv);
	free_string(ret_type);

	return type_info;
}

SERVICE(on_service_called)
{
	struct context_t* context = get_context(service->plugin->game);
	REF(context);
}

/*
 * 0    - directory (string)
 * 1    - callback (callable)
 * 2    - ret_type (type object)
 * 3... - arg_types (type objects)
 */
static PyObject*
Services_create(struct Services* self, PyObject* args)
{
	struct type_info_t* type_info;
	struct service_t* service;
	const char* directory;
	PyObject* callback;
	PyObject* ret_type;
	PyObject* args_as_tuple;
	PyObject* argv_tuple;

	/* get directory, callback and return type */
	if(!PyArg_ParseTuple(args, "sOO", &directory, &callback, &ret_type))
	{
		PyErr_SetString(PyExc_ValueError, "Services must be created with the "
				"arguments <directory string> <callback> <return type>");
		return NULL;
	}

	/* make sure callback is actually callable */
	if(!PyCallable_Check(callback))
	{
		PyErr_SetString(PyExc_ValueError, "The callback isn't callable (you moron)");
		return NULL;
	}

	/*
	 * Slice out the variable argument section from the argument list as a
	 * tuple.
	 */
	args_as_tuple = PySequence_Fast(args, NULL);
	argv_tuple = PyTuple_GetSlice(args_as_tuple, 3, PySequence_Size(args_as_tuple));
	Py_DECREF(args_as_tuple);

	/*  */
	type_info = create_type_info_for_python_callback(ret_type, argv_tuple);
	Py_DECREF(argv_tuple);
	service = service_create(((struct Game*)self->game)->context->plugin,
			directory,
			on_service_called,
			type_info);
	if(!service)
		return NULL;

	Py_RETURN_NONE;
}

/* ------------------------------------------------------------------------- */
static PyObject*
Services_destroy(PyObject* self, PyObject* args)
{
	Py_RETURN_NONE;
}

/* ------------------------------------------------------------------------- */
static PyMethodDef Services_methods[] = {
	{"create", (PyCFunction)Services_create, METH_VARARGS, "creates and registers a new service."},
	{"destroy", Services_destroy, METH_VARARGS, "destroys and unregisters an existing service."},
	{NULL}
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
PyTypeObject ServicesType = {
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
	Services_methods,              /* tp_methods */
	0,                             /* tp_members */
    0,                             /* tp_getset */
    0,                             /* tp_base */
    0,                             /* tp_dict */
    0,                             /* tp_descr_get */
    0,                             /* tp_descr_set */
    0,                             /* tp_dictoffset */
    (initproc)Services_init        /* tp_init */
};
#pragma GCC diagnostic pop
