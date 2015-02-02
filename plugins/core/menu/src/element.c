#include "plugin_menu/element.h"
#include "util/map.h"
#include "util/memory.h"
#include <string.h>

static intptr_t guid = 1;

void
element_init_base(struct element_t* element,
                  element_deinit_derived_func deinit_derived,
                  float x, float y,
                  float width, float height)
{
    element->base.element.id = guid++;
    element->base.element.pos.x = x;
    element->base.element.pos.y = y;
    element->base.element.size.x = width;
    element->base.element.size.y = height;
    element->base.element.deinit_derived = deinit_derived;
    element->base.element.visible = 1;
}

void element_destroy(struct element_t* element)
{
    element->base.element.deinit_derived(element);
}
