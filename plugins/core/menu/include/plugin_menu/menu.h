#include "util/pstdint.h"
#include "util/map.h"
#include "framework/service_api.h"

struct screen_t;
struct game_t;
struct glob_t;

struct menu_t
{
    char* name;
    uint32_t id;
    struct glob_t* glob;
    struct map_t screens;
    struct screen_t* active_screen;
};

void
menu_init(struct glob_t* g);

void
menu_deinit(struct glob_t* g);

struct menu_t*
menu_load(struct glob_t* g, const char* file_name);

void
menu_init_menu(struct menu_t* menu);

void
menu_destroy(struct menu_t* menu);

void
menu_set_active_screen(struct menu_t* menu, const char* screen_name);

SERVICE(menu_load_wrapper);
SERVICE(menu_destroy_wrapper);
SERVICE(menu_set_active_screen_wrapper);
