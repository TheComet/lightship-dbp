#include "plugin_renderer_gl/config.h"
#include "plugin_renderer_gl/text_manager.h"
#include "plugin_renderer_gl/text.h"
#include "plugin_renderer_gl/shader.h"
#include "plugin_renderer_gl/glutils.h"
#include "plugin_renderer_gl/window.h"
#include "util/log.h"
#include "util/memory.h"
#include "util/string.h"
#include "glfw3.h"
#include <math.h>

static void
text_group_sync_with_gpu(struct text_group_t* group);

/* ------------------------------------------------------------------------- */
struct text_t*
text_create(struct text_group_t* text_group, char centered, GLfloat x, GLfloat y, const wchar_t* str)
{
    struct text_t* text;
    text = (struct text_t*)MALLOC(sizeof(struct text_t));
    text->string = malloc_wstring(str);
    text->x = x;
    text->y = y;
    text->is_centered = centered;
    text->visible = 1;
    
    text_group_add_text(text_group, text);
}

/* ------------------------------------------------------------------------- */
void
text_destroy(struct text_group_t* text_group, struct text_t* text)
{
    free_string(text->string);
    FREE(text);
    
    text_group_remove_text(text_group, text);
}

/* ------------------------------------------------------------------------- */
/* TODO remove, there is no access to a list of all stings from within this translation unit */
void
text_destroy_all(struct text_group_t* group)
{
    /* destroy all elements in map *
    MAP_FOR_EACH(&font->static_text_map, struct text_string_instance_t, key, value)
    {
        text_destroy_string_instance(value);
    }
    map_clear_free(&font->static_text_map);*/
    
    /* clear vertices on GPU */
    glBindVertexArray(group->gl.vao);
        glBindBuffer(GL_ARRAY_BUFFER, group->gl.vbo);
            glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, group->gl.ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
    glBindVertexArray(0);
    
    /* reset index counter */
    /*font->gl.static_text_num_indices = 0;*/
}

/* ------------------------------------------------------------------------- */
void
text_set_string(struct font_t* font, struct text_t* text, const wchar_t* string)
{
}

/* ------------------------------------------------------------------------- */
void
text_set_position(struct font_t* font, struct text_t* text, GLfloat x, GLfloat y)
{
}

/* ------------------------------------------------------------------------- */
void
text_show(struct font_t* font, struct text_t* text)
{
    text->visible = 1;
    
    /* TODO refresh with GPU */
}

/* ------------------------------------------------------------------------- */
void
text_hide(struct font_t* font, struct text_t* text)
{
    text->visible = 0;
    
    /* TODO refresh with GPU */
}

/* ------------------------------------------------------------------------- */
static void
text_generate_mesh(struct text_group_t* group, struct text_t* text)
{
    const wchar_t* iterator;
    GLfloat dist_between_chars;
    GLfloat space_dist;
    GLfloat x, y;
    INDEX_DATA_TYPE base_index;
    
    /* start base index at 0 */
    base_index = 0;

    /* distance between characters */
    dist_between_chars = 3.0 / (GLfloat)window_width();
    space_dist = 20.0 / (GLfloat)window_width();
    
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
            info = (struct char_info_t*)map_find(&group->char_info, (intptr_t)*iterator);
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
        x = text->x - (x / 2.0);
        y = text->y;
    }
    else
    {
        x = text->x;
        y = text->y;
    }
    
    /* generate new vertices and insert into text vertex buffer */
    for(iterator = text->string; *iterator; ++iterator)
    {
        struct char_info_t* info;
        struct text_vertex_t* vertex;
        
        /* the space character requires some extra attention */
        if(wcsncmp(iterator, L" ", 1) == 0)
        {
            x += space_dist;
            continue;
        }

        /* lookup character info */
        info = (struct char_info_t*)map_find(&group->char_info, (intptr_t)*iterator);
        if(!info)
        {
            char* buffer[sizeof(wchar_t)+1];
            memcpy(buffer, iterator, sizeof(wchar_t));
            buffer[sizeof(wchar_t)] = '\0';
            llog(LOG_ERROR, PLUGIN_NAME, 3, "Failed to look up character: \"", buffer, "\"");
            continue;
        }

        /* top left vertex */
        vertex = (struct text_vertex_t*)ordered_vector_push_emplace(&text->vertex_buffer);
        vertex->position[0]  = x;
        vertex->position[1]  = y - info->bearing_y;
        vertex->tex_coord[0] = info->uv_left;
        vertex->tex_coord[1] = info->uv_top;
        
        /* top right vertex */
        vertex = (struct text_vertex_t*)ordered_vector_push_emplace(&text->vertex_buffer);
        vertex->position[0]  = x + info->width;
        vertex->position[1]  = y - info->bearing_y;
        vertex->tex_coord[0] = info->uv_left + info->uv_width;
        vertex->tex_coord[1] = info->uv_top;
        
        /* bottom left vertex */
        vertex = (struct text_vertex_t*)ordered_vector_push_emplace(&text->vertex_buffer);
        vertex->position[0]  = x;
        vertex->position[1]  = y - info->bearing_y - info->height;
        vertex->tex_coord[0] = info->uv_left;
        vertex->tex_coord[1] = info->uv_top + info->uv_height;
        
        /* bottom right vertex */
        vertex = (struct text_vertex_t*)ordered_vector_push_emplace(&text->vertex_buffer);
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
}

/* ------------------------------------------------------------------------- */
static void
text_group_sync_with_gpu(struct text_group_t* group)
{
    INDEX_DATA_TYPE base_index = 0;

    /* prepare vertex and index buffers */
    ordered_vector_clear(&group->vertex_buffer);
    ordered_vector_clear(&group->index_buffer);

    /* copy vertex and index data from each text object */
    /* need to make sure indices align correctly */
    {
        MAP_FOR_EACH(&group->texts, struct text_t, key, text)
        {
            if(text->visible)
            {
                intptr_t insertion_index = text->index_buffer.count + 1;
                ordered_vector_push_vector(&group->vertex_buffer, &text->vertex_buffer);
                ordered_vector_push_vector(&group->index_buffer, &text->index_buffer);
                {
                    ORDERED_VECTOR_FOR_EACH_RANGE(&group->index_buffer, INDEX_DATA_TYPE, val, insertion_index, group->index_buffer.count)
                    {
                        (*val) += base_index;
                    }
                }
                base_index += text->index_buffer.count;
            }
        }
    }

    /* upload to GPU */
    glBindVertexArray(group->gl.vao);
        glBindBuffer(GL_ARRAY_BUFFER, group->gl.vbo);
            glBufferData(GL_ARRAY_BUFFER, group->vertex_buffer.count * sizeof(struct text_vertex_t), group->vertex_buffer.data, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, group->index_buffer.count * sizeof(INDEX_DATA_TYPE), group->index_buffer.data, GL_STATIC_DRAW);
    glBindVertexArray(0);
}

/* ------------------------------------------------------------------------- */
/* WRAPPERS */
/* ------------------------------------------------------------------------- */

SERVICE(text_load_font_wrapper)
{
    uint32_t key;
    struct font_t* font;
    SERVICE_EXTRACT_ARGUMENT(0, filename, const char*, const char*);
    SERVICE_EXTRACT_ARGUMENT(1, char_size, uint32_t, uint32_t);

    font = text_load_font(filename, char_size);
    key = map_find_unused_key(&g_wrapper_fonts);
    map_insert(&g_wrapper_fonts, key, font);

    SERVICE_RETURN(key, uint32_t);
}

SERVICE(text_destroy_font_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, font_id, uint32_t, uint32_t);

    struct font_t* font = map_erase(&g_wrapper_fonts, font_id);
    if(font)
        text_destroy_font(font);
}

SERVICE(text_load_characters_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, font_id, uint32_t, uint32_t);
    SERVICE_EXTRACT_ARGUMENT(1, characters, const wchar_t*, const wchar_t*);

    struct font_t* font = map_find(&g_wrapper_fonts, font_id);
    if(font)
        text_load_characters(font, characters);
}

SERVICE(text_add_static_string_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, font_id, uint32_t, uint32_t);
    SERVICE_EXTRACT_ARGUMENT(1, x, float, float);
    SERVICE_EXTRACT_ARGUMENT(2, y, float, float);
    SERVICE_EXTRACT_ARGUMENT(3, str, const wchar_t*, const wchar_t*);
    intptr_t ret_val = -1;

    struct font_t* font = map_find(&g_wrapper_fonts, font_id);
    if(font)
        ret_val = text_add_static_string(font, 0, x, y, str);
    SERVICE_RETURN(ret_val, intptr_t);
}

SERVICE(text_add_static_center_string_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, font_id, uint32_t, uint32_t);
    SERVICE_EXTRACT_ARGUMENT(1, x, float, float);
    SERVICE_EXTRACT_ARGUMENT(2, y, float, float);
    SERVICE_EXTRACT_ARGUMENT(3, str, const wchar_t*, const wchar_t*);
    intptr_t ret_val = -1;

    struct font_t* font = map_find(&g_wrapper_fonts, font_id);
    if(font)
        ret_val = text_add_static_string(font, 1, x, y, str);
    SERVICE_RETURN(ret_val, intptr_t);
}

SERVICE(text_destroy_static_string_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, font_id, uint32_t, uint32_t);
    SERVICE_EXTRACT_ARGUMENT(1, id, intptr_t, intptr_t);
    
    struct font_t* font = map_find(&g_wrapper_fonts, font_id);
    if(font)
        text_destroy_static_string(font, id);
}

SERVICE(text_destroy_all_static_strings_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, font_id, uint32_t, uint32_t);

    struct font_t* font = map_find(&g_wrapper_fonts, font_id);
    if(font)
        text_destroy_all_static_strings(font);
}

SERVICE(text_show_static_string_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, font_id, uint32_t, uint32_t);
    SERVICE_EXTRACT_ARGUMENT(1, id, intptr_t, intptr_t);
    
    struct font_t* font = map_find(&g_wrapper_fonts, font_id);
    if(font)
        text_show_static_string(font, id);
}

SERVICE(text_hide_static_string_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, font_id, uint32_t, uint32_t);
    SERVICE_EXTRACT_ARGUMENT(1, id, intptr_t, intptr_t);
    
    struct font_t* font = map_find(&g_wrapper_fonts, font_id);
    if(font)
        text_hide_static_string(font, id);
}
