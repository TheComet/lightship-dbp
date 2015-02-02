#include "util/pstdint.h"

typedef void (*element_deinit_derived_func)(void*);

struct vec2_t
{
    float x;
    float y;
};

struct action_t
{
    struct service_t* service;
    void** argv;
};

/*!
 * @brief This is a base struct for all menu elements such as buttons, sliders,
 * etc.
 * @note Custom polymorphism, using information from here:
 * http://www.deleveld.dds.nl/inherit.htm
 */
struct element_data_t
{
    char visible;
    intptr_t id;
    intptr_t shapes_normal_id;
    struct vec2_t pos;
    struct vec2_t size;
    struct action_t action;
    element_deinit_derived_func deinit_derived;
};

struct element_t
{
    union
    {
        struct element_data_t element;
    } base;
};

/*!
 * @brief This macro performs like a typesafe case
 */
#define GET_ELEMENT(self) (&(self)->base.element)

void
element_init_base(struct element_t* element,
                  element_deinit_derived_func deinit_derived,
                  float x, float y,
                  float width, float height);

void
element_destroy(struct element_t* element);