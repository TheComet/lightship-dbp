#include "util/pstdint.h"
#include "util/map.h"

struct menu_t
{
    struct map_t screens;
};

struct menu_t*
menu_load(const char* file_name);

void
menu_init_menu(struct menu_t* menu);

void
menu_destroy(struct menu_t* menu);
