#include "plugin_menu/element.h"
#include "util/pstdint.h"
#include "plugin_manager/event_api.h"
#include "plugin_manager/service_api.h"

#define BUTTON_COLOUR_NORMAL 0xFFFFFFFF

struct glob_t;

struct button_data_t
{
    union
    {
        struct element_data_t element;
    } base;
    wchar_t* text;
    uint32_t text_id;
    uint32_t shapes_normal_id;
};

struct button_t
{
    union
    {
        struct button_data_t button;
        struct element_data_t element;
    } base;
};

#define GET_BUTTON(self) (&(self)->base.button)

void button_init(struct glob_t* g);
void button_deinit(struct glob_t* g);

struct button_t*
button_create(struct glob_t* g, const char* text, float x, float y, float width, float height);

void
button_constructor(struct button_t* btn, const char* text, float x, float y, float width, float height);

void
button_destructor(struct button_t* button);

void
button_destroy(struct button_t* button);

void
button_destroy_all(struct glob_t* g);

void
button_free_contents(struct button_t* button);

/*!
 * @brief Tests if the specified point is on top of the specified button.
 * @param button The button object to test. If NULL is specified, all buttons
 * are tested.
 * @param x The x coordinate of the dot.
 * @param y The y coordinate of the dot.
 * @return Returns the button object being collided with, or NULL if there is
 * no collision.
 */
struct button_t*
button_collision(struct glob_t* g, struct button_t* button, float x, float y);

SERVICE(button_create_wrapper);
SERVICE(button_destroy_wrapper);
SERVICE(button_get_text_wrapper);
SERVICE(menu_load_wrapper);

EVENT_LISTENER3(on_mouse_clicked, char mouse_btn, double x, double y);
