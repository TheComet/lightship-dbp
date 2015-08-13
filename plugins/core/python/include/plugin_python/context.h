/*!
 * @file context.h
 * @brief Global data struct.
 *
 * TODO add description why this was necessary to do.
 */

#include "util/pstdint.h"
#include "framework/game.h"

extern uint32_t context_hash;

struct plugin_t;
struct game_t;
struct PyObject;

struct py_lightship_service_t
{
	struct PyObject* register_;
};

struct py_lightship_t
{
	struct py_lightship_service_t service;
};

struct context_t
{
	struct game_t* game;                /* handle of the game object that owns this plugin */
	struct plugin_t* plugin;            /* handle of the plugin object that owns this context */
	/*struct context_events_t events;
	struct context_services_t services;*/
	struct py_lightship_t lightship;
};

void
context_create(struct game_t* game);

void
context_destroy(struct game_t* game);

#define get_context(game) ((struct context_t*)game_get_from_context_store(game, context_hash))
