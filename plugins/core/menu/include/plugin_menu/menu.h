#include "util/pstdint.h"
#include "util/bst_hashed_vector.h"
#include "framework/se_api.h"

struct screen_t;
struct game_t;
struct context_t;

struct menu_t
{
	char* name;
	struct context_t* context;
	struct bsthv_t screens;
	struct screen_t* active_screen;
};

void
menu_init(struct context_t* context);

void
menu_deinit(struct context_t* context);

struct menu_t*
menu_load(struct context_t* context, const char* file_name);

void
menu_init_menu(struct menu_t* menu);

void
menu_destroy(struct menu_t* menu);

void
menu_set_active_screen(struct menu_t* menu, const char* screen_name);

SERVICE(menu_load_wrapper);
SERVICE(menu_destroy_wrapper);
SERVICE(menu_set_active_screen_wrapper);
