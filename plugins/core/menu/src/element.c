#include "plugin_menu/element.h"
#include "plugin_menu/services.h"
#include "util/map.h"
#include "util/memory.h"
#include "util/service_api.h"
#include <string.h>

static intptr_t guid = 1;

void
element_constructor(struct element_t* element,
                  element_destructor_func derived_destructor,
                  float x, float y,
                  float width, float height)
{
    unordered_vector_init_vector(&element->base.element.gl.shapes, sizeof(intptr_t));
    unordered_vector_init_vector(&element->base.element.gl.text, sizeof(struct element_font_text_id_pair_t));
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
    { UNORDERED_VECTOR_FOR_EACH(&element->base.element.gl.shapes, intptr_t, id)
    {
        SERVICE_CALL1(shapes_2d_destroy, SERVICE_NO_RETURN, id);
    }}
    { UNORDERED_VECTOR_FOR_EACH(&element->base.element.gl.text, struct element_font_text_id_pair_t, pair)
    {
        SERVICE_CALL2(text_destroy, SERVICE_NO_RETURN, pair->font_id,pair->text_id);
    }}
    unordered_vector_clear_free(&element->base.element.gl.shapes);
    unordered_vector_clear_free(&element->base.element.gl.text);
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

void
element_add_text(struct element_t* element, intptr_t font_id, intptr_t text_id)
{
    struct element_font_text_id_pair_t* pair = unordered_vector_push_emplace(&element->base.element.gl.text);
    pair->font_id = font_id;
    pair->text_id = text_id;
}

void
element_add_shapes(struct element_t* element, intptr_t shapes_id)
{
    intptr_t* id = unordered_vector_push_emplace(&element->base.element.gl.shapes);
    *id = shapes_id;
}

void
element_show(struct element_t* element)
{
    { UNORDERED_VECTOR_FOR_EACH(&element->base.element.gl.shapes, intptr_t, id)
    {
        SERVICE_CALL1(shapes_2d_show, SERVICE_NO_RETURN, *id);
    }}
    { UNORDERED_VECTOR_FOR_EACH(&element->base.element.gl.text, struct element_font_text_id_pair_t, pair)
    {
        SERVICE_CALL1(text_show, SERVICE_NO_RETURN, pair->text_id);
    }}
    element->base.element.visible = 1;
}

void
element_hide(struct element_t* element)
{
    { UNORDERED_VECTOR_FOR_EACH(&element->base.element.gl.shapes, intptr_t, id)
    {
        SERVICE_CALL1(shapes_2d_hide, SERVICE_NO_RETURN, *id);
    }}
    { UNORDERED_VECTOR_FOR_EACH(&element->base.element.gl.text, struct element_font_text_id_pair_t, pair)
    {
        SERVICE_CALL1(text_hide, SERVICE_NO_RETURN, pair->text_id);
    }}
    element->base.element.visible = 0;
}
