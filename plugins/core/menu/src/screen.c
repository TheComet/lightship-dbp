#include "plugin_menu/screen.h"
#include "plugin_menu/button.h"
#include "util/memory.h"

struct screen_t*
screen_create(void)
{
    struct screen_t* screen = (struct screen_t*)MALLOC(sizeof(struct screen_t));
    screen_init_screen(screen);
    return screen;
}

void
screen_init_screen(struct screen_t* screen)
{
    map_init_map(&screen->buttons);
}

void
screen_destroy(struct screen_t* screen)
{
    MAP_FOR_EACH(&screen->buttons, struct button_t, key, button)
    {
        button_destroy(button);
    }
    map_clear_free(&screen->buttons);
    FREE(screen);
}

void
screen_add_button(struct screen_t* screen, struct button_t* button)
{
}
