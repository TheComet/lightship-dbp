#include "plugin_menu/button.h"
#include "plugin_menu/services.h"
#include "plugin_menu/events.h"
#include "util/map.h"
#include "util/memory.h"
#include "util/string.h"
#include "util/service_api.h"
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
    uint32_t char_size = 9;
    SERVICE_CALL2(text_load_font, &font_id, ttf_filename, char_size);
    SERVICE_CALL2(text_load_characters, SERVICE_NO_RETURN, font_id, SERVICE_NO_ARGUMENT);
    
    map_init_map(&g_buttons);
}

void button_deinit(void)
{
    button_destroy_all();
    map_clear_free(&g_buttons);

    SERVICE_CALL1(text_destroy_font, SERVICE_NO_RETURN, font_id);
}

struct button_t* button_create(const char* text, float x, float y, float width, float height)
{
    struct button_t* btn = (struct button_t*)MALLOC(sizeof(struct button_t));
    btn->pos.x = x;
    btn->pos.y = y;
    btn->size.x = width;
    btn->size.y = height;
    btn->id = map_find_unused_key(&g_buttons);
    map_insert(&g_buttons, btn->id, btn);

    /* copy wchar_t string into button object */
    if(text)
    {
        float offy = y + 0.02;
        /* TODO centering code for text */
        /* TODO instead of passing the raw string, add way to pass a "string instance"
        * which can specify the font and size of the string. */
        btn->text = strtowcs(text);
        SERVICE_CALL4(text_add_static_center_string, &btn->text_id, font_id, x, offy, btn->text);
    }
    else
    {
        btn->text = NULL;
        btn->text_id = 0;
    }

    /* draw box */
    SERVICE_CALL0(shapes_2d_begin, SERVICE_NO_RETURN);
    {
        float x1, y1, x2, y2;
        uint32_t colour = BUTTON_COLOUR_NORMAL;
        x1 = x-width*0.5;
        y1 = y-height*0.5;
        x2 = x+width*0.5;
        y2 = y+height*0.5;
        SERVICE_CALL5(box_2d, SERVICE_NO_RETURN, x1, y1, x2, y2, colour);
    }
    SERVICE_CALL0(shapes_2d_end, &btn->shapes_normal_id);

    return btn;
}

void button_free_contents(struct button_t* button)
{
    SERVICE_CALL1(shapes_2d_destroy, SERVICE_NO_RETURN, button->shapes_normal_id);

    if(button->text)
    {
        SERVICE_CALL2(text_destroy_static_string, SERVICE_NO_RETURN, font_id, button->text_id);
        free_string(button->text);
    }
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

SERVICE(button_create_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, text, const char*, const char*);
    SERVICE_EXTRACT_ARGUMENT(1, x, float, float);
    SERVICE_EXTRACT_ARGUMENT(2, y, float, float);
    SERVICE_EXTRACT_ARGUMENT(3, width, float, float);
    SERVICE_EXTRACT_ARGUMENT(4, height, float, float);
    SERVICE_RETURN(struct button_t*, button_create(text, x, y, width, height));
}

SERVICE(button_destroy_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, id, intptr_t, intptr_t);
    struct button_t* button = map_find(&g_buttons, id);
    if(button)
        button_destroy(button);
}

SERVICE(button_get_text_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, id, intptr_t, intptr_t);
    struct button_t* button = map_find(&g_buttons, id);
    if(button)
        SERVICE_RETURN(wchar_t*, button->text);
    SERVICE_RETURN(wchar_t*, NULL);
}
