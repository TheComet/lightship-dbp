#include "plugin_menu/button.h"
#include "plugin_menu/services.h"
#include "util/unordered_vector.h"
#include "util/memory.h"
#include "util/string.h"

struct unordered_vector_t g_buttons;

static uint32_t guid_counter = 1;

void button_init(void)
{
    unordered_vector_init_vector(&g_buttons, sizeof(struct button_t));
}

void button_deinit(void)
{
    button_destroy_all();
    unordered_vector_clear_free(&g_buttons);
}

static struct button_t* button_get(uint32_t ID)
{
    UNORDERED_VECTOR_FOR_EACH(&g_buttons, struct button_t, button)
    {
        if(button->ID == ID)
            return button;
    }
    return NULL;
}

uint32_t button_create(const char* text, float x, float y, float width, float height)
{
    struct button_t* btn = (struct button_t*)unordered_vector_push_emplace(&g_buttons);
    btn->ID = guid_counter++;
    btn->text = malloc_string(text);
    btn->pos.x = x;
    btn->pos.y = y;
    btn->size.x = width;
    btn->size.y = height;

    shapes_2d_begin();
        box_2d(x-width*0.5, y-height*0.5, x+width*0.5, y+height*0.5, BUTTON_COLOUR_NORMAL);
    btn->shapes_normal_ID = shapes_2d_end();
    
    

    return btn->ID;
}

void button_destroy(uint32_t ID)
{
    UNORDERED_VECTOR_FOR_EACH(&g_buttons, struct button_t, btn)
    {
        if(btn->ID == ID)
        {
            FREE(btn->text);
            shapes_2d_destroy(btn->shapes_normal_ID);
            unordered_vector_erase_element(&g_buttons, btn);
            return;
        }
    }
    
}

void button_destroy_all(void)
{
    UNORDERED_VECTOR_FOR_EACH(&g_buttons, struct button_t, btn)
    {
        FREE(btn->text);
    }
    unordered_vector_clear(&g_buttons);
}
