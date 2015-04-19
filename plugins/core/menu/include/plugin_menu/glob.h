#include "util/pstdint.h"
#include "framework/game.h"

extern uint32_t global_hash;

struct plugin_t;
struct game_t;
struct service_t;

struct glob_element_t
{
    uint32_t guid;
};

struct glob_button_t
{
    uint32_t font_id;
    struct map_t buttons;
};

struct glob_menu_t
{
    uint32_t gid;
    struct map_t menus;
};

struct glob_services_t
{
    struct service_t* yaml_load;
    struct service_t* yaml_get_value;
    struct service_t* yaml_get_dom;
    struct service_t* yaml_destroy;

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

struct glob_events_t
{
    struct event_t* button_clicked;
};

struct glob_t
{
    struct game_t* game;
    struct plugin_t* plugin;
    struct glob_element_t element;
    struct glob_button_t button;
    struct glob_menu_t menu;
    struct glob_services_t services;
    struct glob_events_t events;
};

void
glob_create(struct game_t* game);

void
glob_destroy(struct game_t* game);

#define get_global(game) ((struct glob_t*)game_get_global(game, global_hash))
