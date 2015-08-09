#include <Python.h>

static PyObject*
lightship_test(PyObject* self, PyObject* args)
{
	puts("hello world");
	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef lightship_methods[] = {
	{"test", lightship_test, METH_VARARGS, "docstring"},
	{NULL, NULL, 0, NULL}  /* sentinel */
};

static PyModuleDef lightship_module = {
	PyModuleDef_HEAD_INIT,
	"lightship",       /* module name */
	"",                /* docstring, may be NULL */
	-1,                /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	lightship_methods, /* module methods */
	NULL,              /* m_reload */
	NULL,              /* m_traverse */
	NULL,              /* m_clear */
	NULL               /* m_free */
};

PyMODINIT_FUNC
PyInit_lightship(void)
{
	return PyModule_Create(&lightship_module);
}
