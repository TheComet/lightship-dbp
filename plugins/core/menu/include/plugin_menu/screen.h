#include "util/map.h"

struct element_t;

struct screen_t
{
    struct map_t elements;
};

struct screen_t*
screen_create(void);

void
screen_init_screen(struct screen_t* screen);

void
screen_destroy(struct screen_t* screen);

void
screen_add_element(struct screen_t* screen, struct element_t* element);

void
screen_show(struct screen_t* screen);

void
screen_hide(struct screen_t* screen);
