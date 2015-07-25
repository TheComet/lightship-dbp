#include "plugin_menu/screen.h"
#include "plugin_menu/element.h"
#include "util/memory.h"

/* ------------------------------------------------------------------------- */
struct screen_t*
screen_create(void)
{
	struct screen_t* screen = (struct screen_t*)MALLOC(sizeof(struct screen_t));
	screen_init_screen(screen);
	return screen;
}

/* ------------------------------------------------------------------------- */
void
screen_init_screen(struct screen_t* screen)
{
	bstv_init_bstv(&screen->elements);
}

/* ------------------------------------------------------------------------- */
void
screen_destroy(struct screen_t* screen)
{
	BSTV_FOR_EACH(&screen->elements, struct element_t, key, element)
		element_destroy(element);
	BSTV_END_EACH
	bstv_clear_free(&screen->elements);
	FREE(screen);
}

/* ------------------------------------------------------------------------- */
void
screen_add_element(struct screen_t* screen, struct element_t* element)
{
	bstv_insert(&screen->elements, element->base.element.id, element);
}

/* ------------------------------------------------------------------------- */
void
screen_show(struct screen_t* screen)
{
	BSTV_FOR_EACH(&screen->elements, struct element_t, id, elem)
		element_show(elem);
	BSTV_END_EACH
}

/* ------------------------------------------------------------------------- */
void
screen_hide(struct screen_t* screen)
{
	BSTV_FOR_EACH(&screen->elements, struct element_t, id, elem)
		element_hide(elem);
	BSTV_END_EACH
}
