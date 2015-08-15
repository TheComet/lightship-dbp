#include "plugin_python/lightship_module_game.h"
#include "plugin_python/lightship_module_exceptions.h"
#include "plugin_python/lightship_module_services.h"
#include "plugin_python/context.h"
#include "framework/game.h"
#include <structmember.h>

/*
 * This gets set globally when the plugin first starts and is reset to NULL as
 * soon as the plugin finishes its start routine. This is to allow python
 * scripts to register themselves under the correct context.
 *
 * Since plugins work per-context and not globally (i.e. we don't have access
 * to any of the other game instances) I don't see any other way to do this.
 */
struct context_t* g_injected_context = NULL;

/* method forward declarations */
static PyObject*
Game_new(PyTypeObject* type, PyObject* args, PyObject* kwds);

static void
Game_dealloc(struct Game* self);

static PyObject*
Game_get_name(PyObject* self, PyObject* noargs);

static PyObject*
Game_get_network_role(PyObject* self, PyObject* noargs);

/* ------------------------------------------------------------------------- */
static PyMemberDef Game_members[] = {
	{"service", T_OBJECT_EX, offsetof(struct Game, service), 0, "service"},
	{NULL}
};

static PyMethodDef Game_methods[] = {
	{"get_name", Game_get_name, METH_NOARGS, "Gets the globally unique name of the game object"},
	{"get_network_role", Game_get_network_role, METH_NOARGS, "Gets the network role (either 'host' or 'client'"},
	{NULL}
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
PyTypeObject GameType = {
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
#pragma GCC diagnostic pop

/* ------------------------------------------------------------------------- */
/*
 * Creates a new Game object and injects the active context from PLUGIN_START()
 * into it. The Game object is pushed into the context store.
 *
 * The Game object is given a Services object as an attribute.
 */
static PyObject*
Game_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	struct Game* self;
	PyObject* pass_args;

	if(!g_injected_context)
	{
		PyErr_SetString(lightship_error,
						"Can't instantiate game objects after plugin initialisation because they rely on internal global data.");
		return NULL;
	}

	/* allocate game object */
	self = (struct Game*)type->tp_alloc(type, 0);
	if(!self)
		return NULL;

	/* inject game into python Game object so it can be used later */
	((struct Game*)self)->context = g_injected_context;

	/* make sure game object is actually an instance of GameType */
	if(PyObject_IsInstance((PyObject*)self, (PyObject*)&GameType) == -1)
	{
		PyErr_SetString(lightship_error,
						"Object is not a subclass of lightship.Game");
		Py_DECREF(self);
		return NULL;
	}

	/* instantiate service member */
	pass_args = PyTuple_New(1);
	PyTuple_SetItem(pass_args, 0, (PyObject*)self);
	self->service = PyObject_CallObject((PyObject*)&ServicesType, pass_args);
	Py_DECREF(pass_args);
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
Game_get_name(PyObject* self, PyObject* noargs)
{
	struct Game* game = (struct Game*)self;
	return PyUnicode_FromString(game->context->game->name);
}

/* ------------------------------------------------------------------------- */
static PyObject*
Game_get_network_role(PyObject* self, PyObject* noargs)
{
	struct Game* game = (struct Game*)self;
	PyErr_SetString(lightship_error, "fuck you");
	return NULL;
	if(game->context->game->network_role == GAME_HOST)
		return PyUnicode_FromString("host");
	else
		return PyUnicode_FromString("client");
}
