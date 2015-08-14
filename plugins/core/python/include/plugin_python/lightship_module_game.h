#include <Python.h>

struct game_t;

struct Game
{
	PyObject_HEAD
	PyObject* service;
	struct game_t* game;
};

extern PyTypeObject GameType;
extern struct game_t* g_injected_game;
