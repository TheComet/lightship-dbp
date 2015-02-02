#include "plugin_menu/element.h"
#include "util/pstdint.h"
#include "util/event_api.h"
#include "util/service_api.h"

#define BUTTON_COLOUR_NORMAL 0xFFFFFFFF

struct button_data_t
{
    union
    {
        struct element_data_t element;
    } base;
    wchar_t* text;
    intptr_t text_id;
    intptr_t shapes_normal_id;
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

void button_init(void);
void button_deinit(void);
struct button_t* button_create(const char* text, float x, float y, float width, float height);
void button_destroy(struct button_t* button);
void button_free_contents(struct button_t* button);
void button_destroy_all(void);
wchar_t* button_get_text(intptr_t id);

/*!
 * @brief Tests if the specified point is on top of the specified button.
 * @param button The button object to test. If NULL is specified, all buttons
 * are tested.
 * @param x The x coordinate of the dot.
 * @param y The y coordinate of the dot.
 * @return Returns the button object being collided with, or NULL if there is
 * no collision.
 */
struct button_t* button_collision(struct button_t* button, float x, float y);

SERVICE(button_create_wrapper);
SERVICE(button_destroy_wrapper);
SERVICE(button_get_text_wrapper);
SERVICE(menu_load_wrapper);

EVENT_LISTENER3(on_mouse_clicked, char mouse_btn, double x, double y);
