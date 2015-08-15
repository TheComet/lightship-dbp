#include "plugin_python/lightship_module_exceptions.h"
#include <Python.h>

PyObject* lightship_error;

/* ------------------------------------------------------------------------- */
void
create_lightship_exceptions(PyObject* module)
{
	lightship_error = PyErr_NewException("lightship.Error", NULL, NULL);
	Py_INCREF(lightship_error);
	PyModule_AddObject(module, "Error", lightship_error);
}
