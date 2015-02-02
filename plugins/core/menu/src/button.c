#include "plugin_menu/button.h"
#include "plugin_menu/services.h"
#include "plugin_menu/events.h"
#include "util/map.h"
#include "util/memory.h"
#include "util/string.h"
#include "util/services.h"
#include <string.h>
#include <wchar.h>

static intptr_t font_id = -1;
static struct map_t g_buttons;

#ifdef _DEBUG
static const char* ttf_filename = "../../plugins/core/menu/ttf/DejaVuSans.ttf";
#else
static const char* ttf_filename = "ttf/DejaVuSans.ttf";
#endif

void button_init(void)
{
    uint32_t char_size;

    map_init_map(&g_buttons);

    /* load font and characters */
    char_size = 9;
    SERVICE_CALL2(text_load_font, &font_id, ttf_filename, char_size);
    SERVICE_CALL2(text_load_characters, SERVICE_NO_RETURN, font_id, SERVICE_NO_ARGUMENT);
}

void button_deinit(void)
{
    SERVICE_CALL1(text_destroy_font, SERVICE_NO_RETURN, font_id);
    button_destroy_all();
}

struct button_t*
button_create(const char* text, float x, float y, float width, float height)
{
    struct button_t* btn = (struct button_t*)MALLOC(sizeof(struct button_t));
    memset(btn, 0, sizeof(struct button_t));

    /* base constructor */
    element_constructor((struct element_t*)btn,
                      (element_destructor_func)button_destructor,
                      x, y,
                      width, height);
    
    /* derived constructor */
    button_constructor(btn, text, x, y, width, height);
    
    return btn;
}

void
button_constructor(struct button_t* btn, const char* text, float x, float y, float width, float height)
{

    /* copy wchar_t string into button object */
    if(text)
    {
        float offy = y + 0.02;
        /* TODO centering code for text */
        /* TODO instead of passing the raw string, add way to pass a "string instance"
        * which can specify the font and size of the string. */
        btn->base.button.text = strtowcs(text);
        SERVICE_CALL4(text_add_static_center_string, &btn->base.button.text_id, font_id, x, offy, btn->base.button.text);
        element_add_text((struct element_t*)btn, font_id, btn->base.button.text_id);
    }
    else
    {
        btn->base.button.text = NULL;
        btn->base.button.text_id = 0;
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
    SERVICE_CALL0(shapes_2d_end, &btn->base.button.shapes_normal_id);
    element_add_shapes((struct element_t*)btn, btn->base.button.shapes_normal_id);

    /* insert into internal container of buttons */
    map_insert(&g_buttons, btn->base.element.id, btn);
}

void
button_destructor(struct button_t* button)
{
    button_free_contents(button);
    map_erase_element(&g_buttons, button);
}

void
button_destroy(struct button_t* button)
{
    button_destructor(button);
    element_destructor((struct element_t*)button);
    FREE(button);
}

void
button_destroy_all(void)
{
    MAP_FOR_EACH(&g_buttons, struct button_t, id, button)
    {
        button_destroy(button);
    }
    map_clear_free(&g_buttons);
}

void
button_free_contents(struct button_t* button)
{
    if(button->base.button.text)
    {
        SERVICE_CALL2(text_destroy_static_string, SERVICE_NO_RETURN, font_id, button->base.button.text_id);
        free_string(button->base.button.text);
        if(button->base.element.action.service)
            service_destroy_argument_list(button->base.element.action.service, button->base.element.action.argv);
    }
}

struct button_t*
button_collision(struct button_t* button, float x, float y)
{

    /* test specified button */
    if(button && button->base.element.visible)
    {
        struct element_data_t* elem;
        if(!button->base.element.visible)
            return NULL;
        
        elem = &button->base.element;
        if(x > elem->pos.x - elem->size.x*0.5 && x < elem->pos.x + elem->size.x*0.5)
            if(y > elem->pos.y - elem->size.y*0.5 && y < elem->pos.y + elem->size.y*0.5)
                return button;
        return NULL;
    }
    
    /* test all buttons */
    {
        MAP_FOR_EACH(&g_buttons, struct button_t, id, cur_btn)
        {
            struct element_data_t* elem;
            if(!cur_btn->base.element.visible)
                continue;

            elem = &cur_btn->base.element;
            if(x > elem->pos.x - elem->size.x*0.5 && x < elem->pos.x + elem->size.x*0.5)
                if(y > elem->pos.y - elem->size.y*0.5 && y < elem->pos.y + elem->size.y*0.5)
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
        /* let everything know it was clicked */
        EVENT_FIRE1(evt_button_clicked, button->base.element.id);
        
        /* if button has an action, execute it */
        if(button->base.element.action.service)
        {
            /* Pass vector of args (if there are no args, argv->data should be NULL */
            /* Ignore the return value */
            button->base.element.action.service->exec(SERVICE_NO_RETURN,
                                                      (const void**)button->base.element.action.argv);
        }
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
    SERVICE_RETURN(button_create(text, x, y, width, height), struct button_t*);
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
        SERVICE_RETURN(button->base.button.text, wchar_t*);
    SERVICE_RETURN(NULL, wchar_t*);
}
