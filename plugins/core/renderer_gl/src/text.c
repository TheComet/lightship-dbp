#include "plugin_renderer_gl/config.h"
#include "plugin_renderer_gl/text_manager.h"
#include "plugin_renderer_gl/text.h"
#include "plugin_renderer_gl/window.h"
#include "util/log.h"
#include "util/memory.h"
#include "util/string.h"
#include "glfw3.h"
#include <math.h>

static void
text_generate_mesh(struct text_t* text);

/* ------------------------------------------------------------------------- */
struct text_t*
text_create(struct text_group_t* text_group, char centered, GLfloat x, GLfloat y, const wchar_t* str)
{
    struct text_t* text;
    text = (struct text_t*)MALLOC(sizeof(struct text_t));
    text->string = malloc_wstring(str);
    text->pos.x = x;
    text->pos.y = y;
    text->is_centered = centered;
    text->visible = 1;
    
    ordered_vector_init_vector(&text->vertex_buffer, sizeof(struct vertex_quad_t));
    ordered_vector_init_vector(&text->index_buffer, sizeof(INDEX_DATA_TYPE));
    
    text_group_add_text_object(text_group, text);
    text_generate_mesh(text);
    
    return text;
}

/* ------------------------------------------------------------------------- */
void
text_destroy(struct text_t* text)
{
    if(text->group)
        text_group_remove_text_object(text->group, text);
    
    ordered_vector_clear_free(&text->vertex_buffer);
    ordered_vector_clear_free(&text->index_buffer);

    free_string(text->string);
    FREE(text);
}

/* ------------------------------------------------------------------------- */
void
text_set_string(struct text_t* text, const wchar_t* string)
{
}

/* ------------------------------------------------------------------------- */
void
text_set_position(struct text_t* text, GLfloat x, GLfloat y)
{
}

/* ------------------------------------------------------------------------- */
void
text_set_centered(struct text_t* text, char centered)
{
    text->is_centered = centered;
    text_group_inform_updated_text_object(text->group);
}

/* ------------------------------------------------------------------------- */
void
text_show(struct text_t* text)
{
    text->visible = 1;
    text_group_inform_updated_text_object(text->group);
}

/* ------------------------------------------------------------------------- */
void
text_hide(struct text_t* text)
{
    text->visible = 0;
    text_group_inform_updated_text_object(text->group);
}

/* ------------------------------------------------------------------------- */
static void
text_generate_mesh(struct text_t* text)
{
    const wchar_t* iterator;
    GLfloat dist_between_chars;
    GLfloat space_dist;
    GLfloat x, y;
    INDEX_DATA_TYPE base_index;
    
    /* start base index at 0 */
    base_index = 0;

    /* distance between characters */
    dist_between_chars = 3.0f / (GLfloat)window_width();
    space_dist = 20.0f / (GLfloat)window_width();
    
    /* 
     * If text is centered, figure out total width and set x coordinate
     * accordingly. Otherwise just use x and y values passed in. */
    if(text->is_centered)
    {
        x = 0;
        for(iterator = text->string; *iterator; ++iterator)
        {
            struct char_info_t* info;

            /* the space character requires some extra attention */
            if(wcsncmp(iterator, L" ", 1) == 0)
            {
                x += space_dist;
                continue;
            }
            
            /* lookup character info */
            info = (struct char_info_t*)map_find(&text->group->char_info, (uint32_t)*iterator);
            if(!info)
            {
                char* buffer[sizeof(wchar_t)+1];
                memcpy(buffer, iterator, sizeof(wchar_t));
                buffer[sizeof(wchar_t)] = '\0';
                llog(LOG_ERROR, PLUGIN_NAME, 3, "Failed to look up character: \"", buffer, "\"");
                continue;
            }
            
            /* advance */
            x += info->width + dist_between_chars;
        }
        
        /* x is now the total width of the string in GL screen space. */
        x = text->pos.x - (x / 2.0f);
        y = text->pos.y;
    }
    else
    {
        x = text->pos.x;
        y = text->pos.y;
    }
    
    /* generate new vertices and insert into text vertex buffer */
    for(iterator = text->string; *iterator; ++iterator)
    {
        struct char_info_t* info;
        struct vertex_quad_t* vertex;
        
        /* the space character requires some extra attention */
        if(wcsncmp(iterator, L" ", 1) == 0)
        {
            x += space_dist;
            continue;
        }

        /* lookup character info */
        info = (struct char_info_t*)map_find(&text->group->char_info, (uint32_t)*iterator);
        if(!info)
        {
            char* buffer[sizeof(wchar_t)+1];
            memcpy(buffer, iterator, sizeof(wchar_t));
            buffer[sizeof(wchar_t)] = '\0';
            llog(LOG_ERROR, PLUGIN_NAME, 3, "Failed to look up character: \"", buffer, "\"");
            continue;
        }

        /* top left vertex */
        vertex = (struct vertex_quad_t*)ordered_vector_push_emplace(&text->vertex_buffer);
        vertex->position[0]  = x;
        vertex->position[1]  = y - info->bearing_y;
        vertex->tex_coord[0] = info->uv_left;
        vertex->tex_coord[1] = info->uv_top;
        
        /* top right vertex */
        vertex = (struct vertex_quad_t*)ordered_vector_push_emplace(&text->vertex_buffer);
        vertex->position[0]  = x + info->width;
        vertex->position[1]  = y - info->bearing_y;
        vertex->tex_coord[0] = info->uv_left + info->uv_width;
        vertex->tex_coord[1] = info->uv_top;
        
        /* bottom left vertex */
        vertex = (struct vertex_quad_t*)ordered_vector_push_emplace(&text->vertex_buffer);
        vertex->position[0]  = x;
        vertex->position[1]  = y - info->bearing_y - info->height;
        vertex->tex_coord[0] = info->uv_left;
        vertex->tex_coord[1] = info->uv_top + info->uv_height;
        
        /* bottom right vertex */
        vertex = (struct vertex_quad_t*)ordered_vector_push_emplace(&text->vertex_buffer);
        vertex->position[0]  = x + info->width;
        vertex->position[1]  = y - info->bearing_y - info->height;
        vertex->tex_coord[0] = info->uv_left + info->uv_width;
        vertex->tex_coord[1] = info->uv_top + info->uv_height;
        
        /* generate indices */
        *(INDEX_DATA_TYPE*)ordered_vector_push_emplace(&text->index_buffer) = base_index + 0;
        *(INDEX_DATA_TYPE*)ordered_vector_push_emplace(&text->index_buffer) = base_index + 2;
        *(INDEX_DATA_TYPE*)ordered_vector_push_emplace(&text->index_buffer) = base_index + 1;
        *(INDEX_DATA_TYPE*)ordered_vector_push_emplace(&text->index_buffer) = base_index + 1;
        *(INDEX_DATA_TYPE*)ordered_vector_push_emplace(&text->index_buffer) = base_index + 2;
        *(INDEX_DATA_TYPE*)ordered_vector_push_emplace(&text->index_buffer) = base_index + 3;
    
        x += info->width + dist_between_chars;
        base_index += 4;
    }
    
    /* let text group about mesh update */
    text_group_inform_updated_text_object(text->group);
}
