#include "plugin_python/lightship_module_services.h"

/* -------------------------------------------------------------------------
 * Service object.
 * Callable object allowing python scripts to call internal services of
 * lightship.
 * -------------------------------------------------------------------------*/

struct Service
{
	PyObject_HEAD
};

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

/* -------------------------------------------------------------------------
 * Services object
 * Has static methods for creating and destroying services.
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
	Services_methods               /* tp_methods */
};
