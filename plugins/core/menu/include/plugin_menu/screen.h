#include "util/map.h"

struct element_t;

struct screen_t
{
    struct map_t elements;
};

/*!
 * @brief Creates a new screen object and returns it.
 *
 * A screen is a container for all of the various menu elements. Modifying a
 * screen also modifies all elements that belong to it, e.g. if you hide the
 * screen, it will hide all elements.
 *
 * An element can be inserted into multiple screens simultaneously. Each
 * element holds a ref count on how many screens it belongs to. When that
 * number reaches zero, only then will said element be destroyed.
 * @return Returns a new screen object, or NULL if anything fails.
 */
struct screen_t*
screen_create(void);

/*!
 * Initialises an already allocated screen object.
 * @param screen The object to initialise.
 */
void
screen_init_screen(struct screen_t* screen);

/*!
 * @brief Destroys a screen and decreases the ref count on all of its elements.
 * If the ref count reaches zero, the element in question is destroyed as well.
 * @param screen The screen to destroy.
 */
void
screen_destroy(struct screen_t* screen);

/*!
 * @brief Adds an element to the screen, increasing its ref count.
 * @param screen The screen to add to.
 * @param element The element to add.
 */
void
screen_add_element(struct screen_t* screen, struct element_t* element);

/*!
 * @brief Makes all elements owned by the screen visible.
 * @param screen The screen to show.
 */
void
screen_show(struct screen_t* screen);

/*!
 * @brief Makes all elements owned by the screen hidden.
 * @param screen The screen to hide.
 */
void
screen_hide(struct screen_t* screen);
