#include <Python.h>

struct context_t;

struct Game
{
	PyObject_HEAD
	PyObject* service;
	struct context_t* context;
};

extern PyTypeObject GameType;
extern struct context_t* g_injected_context;
