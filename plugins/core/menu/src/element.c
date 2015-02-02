#include "plugin_menu/element.h"
#include "util/map.h"
#include "util/memory.h"
#include <string.h>

static intptr_t guid = 1;

void
element_constructor(struct element_t* element,
                  element_destructor_func derived_destructor,
                  float x, float y,
                  float width, float height)
{
    element->base.element.id = guid++;
    element->base.element.pos.x = x;
    element->base.element.pos.y = y;
    element->base.element.size.x = width;
    element->base.element.size.y = height;
    element->base.element.derived_destructor = derived_destructor;
    element->base.element.visible = 1;
}

void
element_destructor(struct element_t* element)
{
    /* nothing to do */
}

void
element_destroy(struct element_t* element)
{
    /* destruct derived */
    element->base.element.derived_destructor(element);
    /* destruct base */
    element_destructor(element);

    FREE(element);
}
