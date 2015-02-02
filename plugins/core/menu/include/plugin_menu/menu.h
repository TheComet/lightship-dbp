#include "util/pstdint.h"
#include "util/map.h"
#include "util/service_api.h"

struct screen_t;

struct menu_t
{
    char* name;
    struct map_t screens;
    struct screen_t* active_screen;
};

void
menu_init(void);

void
menu_deinit(void);

struct menu_t*
menu_load(const char* file_name);

void
menu_init_menu(struct menu_t* menu);

void
menu_destroy(struct menu_t* menu);

void
menu_set_active_screen(struct menu_t* menu, const char* name);

SERVICE(menu_load_wrapper);
SERVICE(menu_destroy_wrapper);
SERVICE(menu_set_active_screen_wrapper);
