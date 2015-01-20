#include "plugin_menu/button.h"
#include "plugin_menu/services.h"
#include "plugin_menu/events.h"
#include "util/map.h"
#include "util/memory.h"
#include "util/string.h"
#include <string.h>
#include <wchar.h>

static struct map_t g_buttons;
static intptr_t font_id = -1;

#ifdef _DEBUG
static const char* ttf_filename = "../../plugins/core/menu/ttf/DejaVuSans.ttf";
#else
static const char* ttf_filename = "ttf/DejaVuSans.ttf";
#endif

void button_init(void)
{
    /* load font and characters */
    font_id = text_load_font(ttf_filename, 9);
    text_load_characters(font_id, NULL);
    
    map_init_map(&g_buttons);
}

void button_deinit(void)
{
    button_destroy_all();
    map_clear(&g_buttons);

    text_destroy_font(font_id);
}

struct button_t* button_create(const char* text, float x, float y, float width, float height)
{
    intptr_t len;
    struct button_t* btn = (struct button_t*)MALLOC(sizeof(struct button_t));
    btn->pos.x = x;
    btn->pos.y = y;
    btn->size.x = width;
    btn->size.y = height;
    btn->id = map_find_unused_key(&g_buttons);
    map_insert(&g_buttons, btn->id, btn);

    /* copy wchar_t string into button object */
    len = (strlen(text)+1) * sizeof(wchar_t);
    btn->text = (wchar_t*)MALLOC(len);
    swprintf(btn->text, len, L"%s", text);

    /* draw box */
    shapes_2d_begin();
        box_2d(x-width*0.5, y-height*0.5, x+width*0.5, y+height*0.5, BUTTON_COLOUR_NORMAL);
    btn->shapes_normal_id = shapes_2d_end();

    /* add text to button */
    /* TODO centering code for text */
    /* TODO instead of passing the raw string, add way to pass a "string instance"
     * which can specify the font and size of the string. */
    btn->text_id = text_add_static_center_string(font_id, x, y+0.02, btn->text);

    return btn;
}

void button_free_contents(struct button_t* button)
{
    FREE(button->text);
    shapes_2d_destroy(button->shapes_normal_id);
}

void button_destroy(struct button_t* button)
{
    button_free_contents(button);
    FREE(button);
    map_erase_element(&g_buttons, button);
}

void button_destroy_all(void)
{
    MAP_FOR_EACH(&g_buttons, struct button_t, id, btn)
    {
        button_free_contents(btn);
        FREE(btn);
    }
    map_clear(&g_buttons);
}

struct button_t* button_collision(struct button_t* button, float x, float y)
{

    /* test specified button */
    if(button)
    {
        if(x > button->pos.x - button->size.x*0.5 && x < button->pos.x + button->size.x*0.5)
            if(y > button->pos.y - button->size.y*0.5 && y < button->pos.y + button->size.y*0.5)
                return button;
        return NULL;
    }
    
    /* test all buttons */
    {
        MAP_FOR_EACH(&g_buttons, struct button_t, id, cur_btn)
        {
            if(x > cur_btn->pos.x - cur_btn->size.x*0.5 && x < cur_btn->pos.x + cur_btn->size.x*0.5)
                if(y > cur_btn->pos.y - cur_btn->size.y*0.5 && y < cur_btn->pos.y + cur_btn->size.y*0.5)
                    return cur_btn;
        }
    }
    return NULL;
}

EVENT_LISTENER3(on_mouse_clicked, char mouse_btn, double x, double y)
{
    struct button_t* button = button_collision(NULL, (float)x, (float)y);
    if(button)
    {
        EVENT_FIRE1(evt_button_clicked, button->id);
    }
}

/* ----------------------------------------------------------------------------
 * WRAPPERS
 * --------------------------------------------------------------------------*/

intptr_t
button_create_wrapper(const char* text, float x, float y, float width, float height)
{
    struct button_t* button = button_create(text, x, y, width, height);
    return button->id;
}

void
button_destroy_wrapper(intptr_t id)
{
    struct button_t* button = map_find(&g_buttons, id);
    if(button)
        button_destroy(button);
}

wchar_t*
button_get_text(intptr_t id)
{
    struct button_t* button = map_find(&g_buttons, id);
    if(button)
        return button->text;
    return NULL;
}
