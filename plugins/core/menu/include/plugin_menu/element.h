#include "util/pstdint.h"
#include "util/unordered_vector.h"

struct glob_t;

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

struct element_font_text_id_pair_t
{
    uint32_t font_id;
    uint32_t text_id;
};

struct element_gl_t
{
    struct unordered_vector_t text;     /* holds element_font_text_id_pair_t objects */
    struct unordered_vector_t shapes;   /* holds uint32_t instances */
};

struct element_data_t
{
    char visible;
    uint32_t id;
    struct glob_t* glob;        /* reference to the global struct */
    struct element_gl_t gl;
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

void
element_init(struct glob_t* g);

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
element_constructor(struct glob_t* g,
                    struct element_t* element,
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

void
element_add_text(struct element_t* element, uint32_t font_id, uint32_t text_id);

void
element_add_shapes(struct element_t* element, uint32_t shapes_id);

void
element_set_action(struct element_t* element, struct service_t* service, void** argv);

void
element_show(struct element_t* element);

void
element_hide(struct element_t* element);
