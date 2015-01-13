#include "plugin_renderer_gl/text.h"
#include "plugin_renderer_gl/shader.h"
#include "plugin_renderer_gl/glutils.h"
#include "plugin_renderer_gl/window.h"
#include "util/log.h"
#include "util/memory.h"
#include "util/unordered_vector.h"
#include "util/string.h"
#include "glfw3.h"
#include <wchar.h>
#include <math.h>
#include FT_BITMAP_H

static FT_Library g_lib;
static struct unordered_vector_t g_fonts;
static GLuint g_text_shader_id;

#ifdef _DEBUG
static const char* ttf_prefix = "../../plugins/core/renderer_gl/";
static const char* text_shader_file = "../../plugins/core/renderer_gl/fx/text_2d";
#else
static const char* ttf_prefix = "./";
static const char* text_shader_file = "fx/text_2d";
#endif

static GLuint
to_nearest_pow2(GLuint value)
{
    GLuint nearest = 2;
    while((nearest <<= 1) < value);
    return nearest;
}

char
text_init(void)
{
    FT_Error error;

    /* init freetype */
    error = FT_Init_FreeType(&g_lib);
    if(error)
    {
        llog(LOG_ERROR, 1, "Failed to initialise freetype");
        return 0;
    }
    
    g_text_shader_id = load_shader(text_shader_file);
    
    /* init global vector for storing all loaded fonts */
    unordered_vector_init_vector(&g_fonts, sizeof(struct font_t));
    
    return 1;
}

void
text_deinit(void)
{
    while(g_fonts.count)
        text_destroy_font((struct font_t*)g_fonts.data);
    unordered_vector_clear_free(&g_fonts);

    if(g_text_shader_id)
        glDeleteProgram(g_text_shader_id);printOpenGLError();

    FT_Done_FreeType(g_lib);
}

struct font_t*
text_load_font(const char* filename)
{
    FT_Error error;
    char* ttf_filename;
    struct font_t* font;
    
    /* create new font object */
    font = (struct font_t*)unordered_vector_push_emplace(&g_fonts);
    memset(font, 0, sizeof(struct font_t));
    map_init_map(&font->char_map);
    map_init_map(&font->static_text_map);
    
    /* add path prefix to filename */
    ttf_filename = cat_strings(2, ttf_prefix, filename);
    
    /* if the program breaks from this for-loop, something went wrong. */
    for(;;)
    {

        /* load face */
        error = FT_New_Face(g_lib, ttf_filename, 0, &font->face);
        if(error == FT_Err_Unknown_File_Format)
        {
            llog(LOG_ERROR, 3, "The font file \"", ttf_filename, "\" could be opened and read, but it appears that its font format is unsupported");
            break;
        }
        else if(error)
        {
            llog(LOG_ERROR, 3, "Failed to open font file \"", ttf_filename, "\"");
            break;
        }
        
        /* set default size 9 */
        error = FT_Set_Char_Size(font->face, 0, TO_26DOT6(9), 300, 300);
        if(error)
        {
            llog(LOG_ERROR, 1, "Failed to set the character size to 9");
            break;
        }
        
        /* generate VAO, VBO, VIO, and Texture buffer */
        glGenVertexArrays(1, &font->gl.vao);printOpenGLError();
        glBindVertexArray(font->gl.vao);printOpenGLError();
            glGenBuffers(1, &font->gl.vbo);printOpenGLError();
            glBindBuffer(GL_ARRAY_BUFFER, font->gl.vbo);printOpenGLError();
                glEnableVertexAttribArray(0);printOpenGLError();
                glVertexAttribPointer(0,                /* attribute 0 */
                                      2,                /* size, position[2] */
                                      GL_FLOAT,         /* type */
                                      GL_FALSE,         /* normalise? */
                                      sizeof(struct text_vertex_t),
                                      (void*)offsetof(struct text_vertex_t, position));printOpenGLError();
                glEnableVertexAttribArray(1);printOpenGLError();
                glVertexAttribPointer(1,                /* attribute 1 */
                                      2,                /* size, tex_coord[2] */
                                      GL_FLOAT,         /* type */
                                      GL_FALSE,         /* normalise? */
                                      sizeof(struct text_vertex_t),
                                      (void*)offsetof(struct text_vertex_t, tex_coord));printOpenGLError();
                glEnableVertexAttribArray(2);printOpenGLError();
                glVertexAttribPointer(2,                /* attribute 2 */
                                      4,                /* size, diffuse[4] */
                                      GL_FLOAT,         /* type */
                                      GL_FALSE,         /* normalise? */
                                      sizeof(struct text_vertex_t),
                                      (void*)offsetof(struct text_vertex_t, diffuse));printOpenGLError();
            glGenBuffers(1, &font->gl.vio);printOpenGLError();
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, font->gl.vio);printOpenGLError();
            glGenTextures(1, &font->gl.tex);printOpenGLError();
            glBindTexture(GL_TEXTURE_2D, font->gl.tex);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);printOpenGLError();
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);printOpenGLError();
        glBindVertexArray(0);printOpenGLError();
        
        /*
         * If the program reaches this point, it means loading the font was
         * successful.
         */

        /* clean up and return */
        FREE(ttf_filename);
        return font;
    }

    /*
     * If the program reaches this point, it means something went wrong.
     * Clean up and return.
     */
    FREE(ttf_filename);
    unordered_vector_erase_element(&g_fonts, font);
    return NULL;
}

void
text_destroy_font(struct font_t* font)
{
    if(map_count(&font->static_text_map))
    {
        MAP_FOR_EACH(&font->static_text_map, wchar_t, key, value)
        {
            FREE(value);
        }
        map_clear(&font->static_text_map);
    }
    {
        MAP_FOR_EACH(&font->char_map, struct text_char_info_t, key, value)
        {
            FREE(value);
        }
        map_clear(&font->char_map);
    }
    glDeleteTextures(1, &font->gl.tex);printOpenGLError();
    glDeleteBuffers(1, &font->gl.vio);printOpenGLError();
    glDeleteBuffers(1, &font->gl.vbo);printOpenGLError();
    glDeleteVertexArrays(1, &font->gl.vao);printOpenGLError();
    FT_Done_Face(font->face);
    unordered_vector_erase_element(&g_fonts, font);
}

void
text_load_characters(struct font_t* font, const wchar_t* characters)
{
    const wchar_t* iterator;
    const wchar_t* null_terminator = L'\0';
    struct unordered_vector_t sorted_chars;

    /* filter list to eliminate duplicates */
    for(iterator = characters; *iterator; ++iterator)
    {
        wchar_t* character = map_find(&font->char_map, (intptr_t)*iterator);
        if(character)
            continue;

        /* add character to character map */
        map_insert(&font->char_map, (intptr_t)*iterator, NULL);
    }

    /* generate list of characters to be loaded */
    unordered_vector_init_vector(&sorted_chars, sizeof(wchar_t));
    {
        MAP_FOR_EACH(&font->char_map, void, key, value)
        {
            unordered_vector_push(&sorted_chars, &key);
        }
        unordered_vector_push(&sorted_chars, &null_terminator);
    }
    if(sorted_chars.count > 1)
        text_load_atlass(font, (wchar_t*)sorted_chars.data);
    
    unordered_vector_clear_free(&sorted_chars);
}

void
text_load_atlass(struct font_t* font, const wchar_t* characters)
{
    const wchar_t* iterator;
    FT_Error error;
    unsigned int pen;
    unsigned int tex_width, tex_height, glyph_offset_y;
    GLuint* buffer = NULL;
    
    /*
     * First, we need to:
     *   + find the glyph with the maximum height.
     *   + find the glyph with the maximum vertical Y bearing.
     *   + accumulate all advance spacings.
     * This will be used to determine the dimensions of the atlass as well as
     * the correct Y offsets of each glyph when rendering to the atlass.
     * 
     * Report any missing glyphs to the log.
     */
    tex_width = 0;
    tex_height = 0;
    glyph_offset_y = 0;
    for(iterator = characters; *iterator; ++iterator)
    {
        unsigned int height;
        unsigned int offset;
        unsigned int min_advance;

        error = FT_Load_Char(font->face, *iterator, FT_LOAD_RENDER);
        if(error)
        {
            char* buffer[sizeof(wchar_t)+1];
            memcpy(buffer, iterator, sizeof(wchar_t));
            buffer[sizeof(wchar_t)] = '\0';
            llog(LOG_ERROR, 3, "Failed to load glyph \"", buffer, "\"");
            continue;
        }
        
        /* offset */
        offset = TO_PIXELS(font->face->glyph->metrics.horiBearingY);
        if(glyph_offset_y < offset)
            glyph_offset_y = offset;
        
        /* width */
        min_advance = 2;
        tex_width += TO_PIXELS(font->face->glyph->advance.x) + min_advance;
        
        /* height */
        height = font->face->glyph->bitmap.rows;
        if(tex_height < height)
            tex_height = height;
    }

    /*
     * With the maximum width and height determined, calculate the width and
     * height of the target texture atlass and allocate memory for it.
     */
    tex_width = to_nearest_pow2(tex_width);
    tex_height = to_nearest_pow2(tex_height);
    buffer = (GLuint*)MALLOC(tex_width * tex_height * sizeof(GLuint));
    memset(buffer, 0xFFFFFF00, tex_width * tex_height * sizeof(GLuint));
    
    /*
     * Render glyphs onto atlass
     */
    pen = 0;
    for(iterator = characters; *iterator; ++iterator)
    {
        GLubyte* bmp_ptr;
        unsigned int bmp_width, bmp_height, bmp_offset_y, x, y;
        struct text_char_info_t* character;

        /* load rendered character, ignore any errors as they were already reported earlier */
        error = FT_Load_Char(font->face, *iterator, FT_LOAD_RENDER);
        if(error)
            continue;
        
        bmp_ptr = (GLubyte*)font->face->glyph->bitmap.buffer;
        bmp_width = font->face->glyph->bitmap.width;
        bmp_height = font->face->glyph->bitmap.rows;
        bmp_offset_y = glyph_offset_y-TO_PIXELS(font->face->glyph->metrics.horiBearingY);
        
        /* need to convert whatever pixel mode bitmap has to RGBA */
        switch(font->face->glyph->bitmap.pixel_mode)
        {
            /* 8-bit pixel modes */
            case FT_PIXEL_MODE_GRAY:
            case FT_PIXEL_MODE_LCD:
            case FT_PIXEL_MODE_LCD_V:
                for(y = 0; y != bmp_height; ++y)
                {
                    GLuint* buffer_ptr = buffer + ((bmp_offset_y+y) * tex_width) + pen;

                    for(x = 0; x != bmp_width; ++x)
                    {
                        /* convert single byte to RGBA, set A to 0xFF */
                        GLuint target_colour = *bmp_ptr++;
                        *buffer_ptr++ = (target_colour <<  8) |
                                        (target_colour << 16) |
                                        (target_colour << 24) |
                                        0x000000FF;
                    }
                }
                break;
            
            /* other pixel modes */
            default:
                llog(LOG_ERROR, 1, "Glyph bitmap has unsupported format (conversion to RGBA needs implementing)");
                break;
        }
        
        /* save offsets as UV data */
        character = (struct text_char_info_t*)MALLOC(sizeof(struct text_char_info_t));
        character->uv_left = (GLfloat)(pen         ) / (GLfloat)tex_width;
        character->uv_top  = (GLfloat)(bmp_offset_y) / (GLfloat)tex_height;
        /* save width and height in texture space */
        character->uv_width  = (GLfloat)bmp_width  / (GLfloat)tex_width;
        character->uv_height = (GLfloat)bmp_height / (GLfloat)tex_height;
        /* save width and height in screen space */
        character->width  = (GLfloat)bmp_width  * 2.0 / ((GLfloat)window_width());
        character->height = (GLfloat)bmp_height * 2.0 / ((GLfloat)window_height());
        map_set(&font->char_map, (intptr_t)*iterator, character);
        
        /* advance pen */
        pen += TO_PIXELS(font->face->glyph->advance.x) + 2;
    }
    
    /*
     * Hand atlass to OpenGL and clean up.
     */
    glBindVertexArray(font->gl.vao);printOpenGLError();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer);printOpenGLError();
    glBindVertexArray(0);printOpenGLError();
    FREE(buffer);
}

void
text_add_static(struct font_t* font, GLfloat x, GLfloat y, const wchar_t* str)
{
    struct unordered_vector_t vertex_buffer;
    struct unordered_vector_t index_buffer;
    GLfloat x_coord;
    const wchar_t* iterator;
    intptr_t base_index;
    
    /* set current x coordinate and base index for new indices */
    x_coord = x;
    base_index = font->static_text_map.vector.count;
    
    /* generate new vertices and insert into static text vertex buffer */
    unordered_vector_init_vector(&vertex_buffer, sizeof(struct text_vertex_t));
    unordered_vector_init_vector(&index_buffer, sizeof(INDEX_DATA_TYPE));
    for(iterator = str; *iterator; ++iterator)
    {
        struct text_char_info_t* info;
        struct text_vertex_t* vertex;

        /* lookup character info */
        info = (struct text_char_info_t*)map_find(&font->char_map, (intptr_t)*iterator);
        if(!info)
        {
            llog(LOG_ERROR, 1, "Failed to look up character");
            continue;
        }
        
        info->uv_left = 0;
        info->uv_top = 0;
        info->uv_width = 1;
        info->uv_height = 1;

        /* top left vertex */
        vertex = (struct text_vertex_t*)unordered_vector_push_emplace(&vertex_buffer);
        vertex->position[0]  = x_coord;
        vertex->position[1]  = y + info->height;
        vertex->tex_coord[0] = info->uv_left;
        vertex->tex_coord[1] = info->uv_top;
        vertex->diffuse[0]   = 1.0;
        vertex->diffuse[1]   = 1.0;
        vertex->diffuse[2]   = 1.0;
        vertex->diffuse[3]   = 1.0;
        
        /* top right vertex */
        vertex = (struct text_vertex_t*)unordered_vector_push_emplace(&vertex_buffer);
        vertex->position[0]  = x_coord + info->width;
        vertex->position[1]  = y + info->height;
        vertex->tex_coord[0] = info->uv_left + info->uv_width;
        vertex->tex_coord[1] = info->uv_top;
        vertex->diffuse[0]   = 1.0;
        vertex->diffuse[1]   = 1.0;
        vertex->diffuse[2]   = 1.0;
        vertex->diffuse[3]   = 1.0;
        
        /* bottom left vertex */
        vertex = (struct text_vertex_t*)unordered_vector_push_emplace(&vertex_buffer);
        vertex->position[0]  = x_coord;
        vertex->position[1]  = y;
        vertex->tex_coord[0] = info->uv_left;
        vertex->tex_coord[1] = info->uv_top + info->uv_height;
        vertex->diffuse[0]   = 1.0;
        vertex->diffuse[1]   = 1.0;
        vertex->diffuse[2]   = 1.0;
        vertex->diffuse[3]   = 1.0;
        
        /* bottom right vertex */
        vertex = (struct text_vertex_t*)unordered_vector_push_emplace(&vertex_buffer);
        vertex->position[0]  = x_coord + info->width;
        vertex->position[1]  = y;
        vertex->tex_coord[0] = info->uv_left + info->uv_width;
        vertex->tex_coord[1] = info->uv_top + info->uv_height;
        vertex->diffuse[0]   = 1.0;
        vertex->diffuse[1]   = 1.0;
        vertex->diffuse[2]   = 1.0;
        vertex->diffuse[3]   = 1.0;
        
        /* generate indices */
        *(INDEX_DATA_TYPE*)unordered_vector_push_emplace(&index_buffer) = base_index + 0;
        *(INDEX_DATA_TYPE*)unordered_vector_push_emplace(&index_buffer) = base_index + 2;
        *(INDEX_DATA_TYPE*)unordered_vector_push_emplace(&index_buffer) = base_index + 1;
        *(INDEX_DATA_TYPE*)unordered_vector_push_emplace(&index_buffer) = base_index + 1;
        *(INDEX_DATA_TYPE*)unordered_vector_push_emplace(&index_buffer) = base_index + 2;
        *(INDEX_DATA_TYPE*)unordered_vector_push_emplace(&index_buffer) = base_index + 3;
    
        x_coord += info->width;
        base_index += 6;
    }
    
    /* upload to GPU */
    glBindVertexArray(font->gl.vao);
        glBufferData(GL_ARRAY_BUFFER, vertex_buffer.count * sizeof(struct text_vertex_t), vertex_buffer.data, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer.count * sizeof(INDEX_DATA_TYPE), index_buffer.data, GL_STATIC_DRAW);
    glBindVertexArray(0);
    
    /* set number of indices */
    font->gl.static_text_num_indices = index_buffer.count;
    
    /* clean up */
    unordered_vector_clear_free(&vertex_buffer);
    unordered_vector_clear_free(&index_buffer);
}

void
text_draw(void)
{
    /*glEnable(GL_BLEND);*/
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);printOpenGLError();
    glUseProgram(g_text_shader_id);printOpenGLError();
    {
        UNORDERED_VECTOR_FOR_EACH(&g_fonts, struct font_t, font)
        {
            glBindVertexArray(font->gl.vao);printOpenGLError();
                glDrawElements(GL_LINES, font->gl.static_text_num_indices, GL_UNSIGNED_SHORT, NULL);printOpenGLError();
        }
    }
    glDisable(GL_BLEND);printOpenGLError();
    glBindVertexArray(0);printOpenGLError();
}
