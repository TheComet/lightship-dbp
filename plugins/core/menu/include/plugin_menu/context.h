#include "util/pstdint.h"
#include "framework/game.h"

extern uint32_t context_hash;

struct plugin_t;
struct game_t;
struct service_t;

struct context_element_t
{
	uint32_t guid;
};

struct context_button_t
{
	uint32_t font_id;
	struct bstv_t buttons;
};

struct context_menu_t
{
	uint32_t gid;
	struct bsthv_t menus;     /* maps menu IDs to menu objects */
};

struct context_services_t
{
	struct service_t* shapes_2d_begin;
	struct service_t* shapes_2d_end;
	struct service_t* shapes_2d_destroy;
	struct service_t* line_2d;
	struct service_t* box_2d;
	struct service_t* shapes_2d_show;
	struct service_t* shapes_2d_hide;

	struct service_t* text_group_create;
	struct service_t* text_group_destroy;
	struct service_t* text_group_load_character_set;
	struct service_t* text_create;
	struct service_t* text_destroy;
	struct service_t* text_show;
	struct service_t* text_hide;
};

struct context_events_t
{
	struct event_t* button_clicked;
};

struct context_t
{
	struct game_t* game;
	struct plugin_t* plugin;
	struct context_element_t element;
	struct context_button_t button;
	struct context_menu_t menu;
	struct context_services_t services;
	struct context_events_t events;
};

void
context_create(struct game_t* game);

void
context_destroy(struct game_t* game);

#define get_context(game) ((struct context_t*)game_get_from_context_store(game, context_hash))
