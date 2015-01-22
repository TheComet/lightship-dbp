#include "util/map.h"

struct button_t;

struct screen_t
{
    struct map_t buttons;
};

struct screen_t*
screen_create(void);

void
screen_init_screen(struct screen_t* screen);

void
screen_destroy(struct screen_t* screen);

void
screen_add_button(struct screen_t* screen, struct button_t* button);
