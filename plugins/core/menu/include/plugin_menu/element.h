#include "util/pstdint.h"

typedef void (*element_destructor_func)(void*);

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

struct element_data_t
{
    char visible;
    intptr_t id;
    intptr_t shapes_normal_id;
    struct vec2_t pos;
    struct vec2_t size;
    struct action_t action;
    element_destructor_func derived_destructor;
};

/*!
 * @brief This is a base struct for all menu elements such as buttons, sliders,
 * etc.
 * @note Custom polymorphism, using information from here:
 * http://www.deleveld.dds.nl/inherit.htm
 */
struct element_t
{
    union
    {
        struct element_data_t element;
    } base;
};

/*!
 * @brief Initialises the base struct.
 * @param element The element to initialise.
 * @param derived_destructor Function to the derived struct's destructor.
 * @note The destructor must NOT free the object. This is handled separately.
 * @param x The x coordinate of the menu element in GL screen space.
 * @param y The y coordinate of the menu element in GL screen space.
 * @param width The width of the menu element in GL screen space.
 * @param height The height of the menu element in GL screen space.
 */
void
element_constructor(struct element_t* element,
                    element_destructor_func derived_destructor,
                    float x, float y,
                    float width, float height);

/*!
 * @brief Cleans up the base struct.
 */
void
element_destructor(struct element_t* element);

/*!
 * @brief Calls the derived destructor, then calls the base destructor, and
 * finally frees the object.
 * @param element The element to destroy.
 */
void
element_destroy(struct element_t* element);
