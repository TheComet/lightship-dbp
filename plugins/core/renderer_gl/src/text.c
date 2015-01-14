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
static struct map_t g_wrapper_fonts;
static GLuint g_text_shader_id;

static const wchar_t* g_default_characters =
L"abcdefghijklmnopqrstuvwxyz"
L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
L"1234567890"
L" +-*/!?'^\"$%&()[]{}#@~,.";

#ifdef _DEBUG
static const char* ttf_prefix = "../../plugins/core/renderer_gl/";
static const char* text_shader_file = "../../plugins/core/renderer_gl/fx/text_2d";
#else
static const char* ttf_prefix = "./";
static const char* text_shader_file = "fx/text_2d";
#endif

/*!
 * @brief Rounds a number up to the nearest power of 2.
 * @param value The value to round up.
 * @return The rounded value.
 */
static GLuint
to_nearest_pow2(GLuint value)
{
    GLuint nearest = 2;
    while((nearest <<= 1) < value);
    return nearest;
}

/*!
 * @brief Takes a string, generates vertex and index data, and appends it to
 * the respective buffers.
 * @param font The font to use to generate the data from.
 * @param x The x coordinate of the string in GL screen space.
 * @param y The y coordinate of the string in GL screen space.
 * @param vertex_buffer The unsigned_vector to append vertices to.
 * @param index_buffer The unsigned_vector to append indices to.
 * @param str The wide character string to generate the data from.
 */
static void
text_convert_text_to_vbo(struct font_t* font,
                              GLfloat x,
                              GLfloat y,
                              struct unordered_vector_t* vertex_buffer,
                              struct unordered_vector_t* index_buffer,
                              const wchar_t* str);

/*!
 * @brief Creates a new text_string_instance_t object.
 * @param font The font to create the instance for.
 * @param x X coordinate in GL screen space of the string.
 * @param y Y coordiante in GL screen space of the string.
 * @param str The string to copy into the instance.
 */
static struct text_string_instance_t*
text_create_string_instance(struct font_t* font, char centered, GLfloat x, GLfloat y, const wchar_t* str)
{
    struct text_string_instance_t* instance;
    wchar_t* str_buffer;
    str_buffer = (wchar_t*)MALLOC((wcslen(str) + 1) * sizeof(wchar_t));
    wcscpy(str_buffer, str);
    instance = (struct text_string_instance_t*)MALLOC(sizeof(struct text_string_instance_t));
    instance->font = font;
    instance->text = str_buffer;
    instance->x = x;
    instance->y = y;
    instance->is_centered = centered;
    
    return instance;
}

/*!
 * @brief Destroys a text_string_instance_t object.
 * @param instance The instance to destroy.
 */
static void
text_destroy_string_instance(struct text_string_instance_t* instance)
{
    FREE(instance->text);
    FREE(instance);
}

/* ------------------------------------------------------------------------- */
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
    map_init_map(&g_wrapper_fonts);
    
    return 1;
}

/* ------------------------------------------------------------------------- */
void
text_deinit(void)
{
    map_clear(&g_wrapper_fonts);

    while(g_fonts.count)
        text_destroy_font((struct font_t*)g_fonts.data);
    unordered_vector_clear_free(&g_fonts);

    if(g_text_shader_id)
        glDeleteProgram(g_text_shader_id);printOpenGLError();

    FT_Done_FreeType(g_lib);
}

/* ------------------------------------------------------------------------- */
struct font_t*
text_load_font(const char* filename, uint32_t char_size)
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
        
        /* set character size */
        error = FT_Set_Char_Size(font->face, TO_26DOT6(char_size), 0, 300, 300);
        if(error)
        {
            llog(LOG_ERROR, 1, "Failed to set the character size");
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

/* ------------------------------------------------------------------------- */
void
text_destroy_font(struct font_t* font)
{
    /* destroy static text map */
    text_destroy_all_static_strings(font);

    /* destroy character map */
    {
        MAP_FOR_EACH(&font->char_map, struct text_char_info_t, key, value)
        {
            if(value)
                FREE(value);
        }
        map_clear(&font->char_map);
    }
    
    /* clean up GL stuff */
    glDeleteTextures(1, &font->gl.tex);printOpenGLError();
    glDeleteBuffers(1, &font->gl.vio);printOpenGLError();
    glDeleteBuffers(1, &font->gl.vbo);printOpenGLError();
    glDeleteVertexArrays(1, &font->gl.vao);printOpenGLError();
    
    /* clean up freetype stuff */
    FT_Done_Face(font->face);
    
    /* finally, destroy font object */
    unordered_vector_erase_element(&g_fonts, font);
}

/* ------------------------------------------------------------------------- */
void
text_load_characters(struct font_t* font, const wchar_t* characters)
{
    const wchar_t* iterator;
    const wchar_t* null_terminator = L'\0';
    struct unordered_vector_t sorted_chars;
    
    /* if characters are NULL, use default set */
    if(!characters)
        characters = g_default_characters;

    /* filter list to eliminate duplicates */
    for(iterator = characters; *iterator; ++iterator)
    {
        if(map_key_exists(&font->char_map, (intptr_t)*iterator))
            continue;

        /* add character to character map */
        map_insert(&font->char_map, (intptr_t)*iterator, NULL);
    }

    /* generate list of characters to be loaded */
    unordered_vector_init_vector(&sorted_chars, sizeof(wchar_t));
    {
        MAP_FOR_EACH(&font->char_map, void, key, value)
        {
            /* XXX is wchar_t guaranteed to be smaller than intptr_t? */
            unordered_vector_push(&sorted_chars, &key);
        }
        unordered_vector_push(&sorted_chars, &null_terminator);
    }

    if(sorted_chars.count > 1)
        text_load_atlass(font, (wchar_t*)sorted_chars.data);
    
    unordered_vector_clear_free(&sorted_chars);
}

/* ------------------------------------------------------------------------- */
void
text_load_atlass(struct font_t* font, const wchar_t* characters)
{
    const wchar_t* iterator;
    FT_Error error;
    unsigned int pen;
    unsigned int tex_width, tex_height, glyph_offset_y;
    unsigned int min_advance;
    GLuint* buffer = NULL;
    
    /* the number of pixels to place between each glyph */
    /* TODO for some reason this isn't actually true with all glyphs. Who knows why. */
    min_advance = 2;
    
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
    tex_height = to_nearest_pow2(tex_height + glyph_offset_y);
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
        struct text_char_info_t* char_info;

        /* load rendered character, ignore any errors as they were already reported earlier */
        error = FT_Load_Char(font->face, *iterator, FT_LOAD_RENDER);
        if(error)
            continue;
        
        bmp_ptr = (GLubyte*)font->face->glyph->bitmap.buffer;
        bmp_width = font->face->glyph->bitmap.width;
        bmp_height = font->face->glyph->bitmap.rows;
        bmp_offset_y = glyph_offset_y - TO_PIXELS(font->face->glyph->metrics.horiBearingY);
        
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
                                        
                        /* TODO remove */
                        if((GLuint)buffer_ptr - (GLuint)buffer >= tex_width * tex_height * sizeof(GLuint))
                            printf("oh shit\n");
                    }
                }
                break;

            /* other pixel modes */
            default:
                llog(LOG_ERROR, 1, "Glyph bitmap has unsupported format (conversion to RGBA needs implementing)");
                break;
        }

        /* save offsets as UV data */
        char_info = (struct text_char_info_t*)MALLOC(sizeof(struct text_char_info_t));
        char_info->uv_left = (GLfloat)(pen         ) / (GLfloat)tex_width;
        char_info->uv_top  = (GLfloat)(bmp_offset_y) / (GLfloat)tex_height;
        /* save width and height in texture space */
        char_info->uv_width  = (GLfloat)bmp_width    / (GLfloat)tex_width;
        char_info->uv_height = (GLfloat)bmp_height   / (GLfloat)tex_height;
        /* save width and height in screen space */
        char_info->width     = (GLfloat)bmp_width    / (GLfloat)window_width();
        char_info->height    = (GLfloat)bmp_height   / (GLfloat)window_height();
        char_info->bearing_y = (GLfloat)bmp_offset_y / (GLfloat)window_height();
        map_set(&font->char_map, (intptr_t)*iterator, char_info);

        /* advance pen */
        pen += TO_PIXELS(font->face->glyph->advance.x) + min_advance;
    }

    /*
     * Hand atlass to OpenGL and clean up.
     */
    glBindVertexArray(font->gl.vao);printOpenGLError();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer);printOpenGLError();
    glBindVertexArray(0);printOpenGLError();
    FREE(buffer);
}

/* ------------------------------------------------------------------------- */
intptr_t
text_add_static_string(struct font_t* font, char centered, GLfloat x, GLfloat y, const wchar_t* str)
{
    struct unordered_vector_t vertex_buffer;
    struct unordered_vector_t index_buffer;
    intptr_t text_key;

    /* create new string instance and insert into map */
    if(str)
    {
        struct text_string_instance_t* str_instance;
        str_instance = text_create_string_instance(font, centered, x, y, str);
        text_key = map_find_unused_key(&font->static_text_map);
        map_insert(&font->static_text_map, text_key, str_instance);
    }

    /* init vertex and index buffers */
    unordered_vector_init_vector(&vertex_buffer, sizeof(struct text_vertex_t));
    unordered_vector_init_vector(&index_buffer, sizeof(INDEX_DATA_TYPE));

    /* convert all strings in static text map to vertex and index data */
    {
        MAP_FOR_EACH(&font->static_text_map, struct text_string_instance_t, key, value)
        {
            text_convert_text_to_vbo(font, value->x, value->y, &vertex_buffer, &index_buffer, value->text);
        }
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
    
    return text_key;
}

/* ------------------------------------------------------------------------- */
void
text_destroy_static_string(struct font_t* font, intptr_t ID)
{
    struct text_string_instance_t* instance;
    instance = (struct text_string_instance_t*)map_erase(&font->static_text_map, ID);
    if(instance)
    {
        text_destroy_string_instance(instance);
        text_add_static_string(font, 0, 0, 0, NULL);
    }
}

/* ------------------------------------------------------------------------- */
void
text_destroy_all_static_strings(struct font_t* font)
{
    /* destroy all elements in map */
    MAP_FOR_EACH(&font->static_text_map, struct text_string_instance_t, key, value)
    {
        text_destroy_string_instance(value);
    }
    map_clear(&font->static_text_map);
    
    /* clear vertices on GPU */
    glBindVertexArray(font->gl.vao);
        glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
    glBindVertexArray(0);
    
    /* reset index counter */
    font->gl.static_text_num_indices = 0;
}

/* ------------------------------------------------------------------------- */
static void
text_convert_text_to_vbo(struct font_t* font,
                              GLfloat x,
                              GLfloat y,
                              struct unordered_vector_t* vertex_buffer,
                              struct unordered_vector_t* index_buffer,
                              const wchar_t* str)
{
    const wchar_t* iterator;
    GLfloat dist_between_chars;
    GLfloat space_dist;
    GLfloat x_coord;
    INDEX_DATA_TYPE base_index;
    
    x_coord = x;
    base_index = vertex_buffer->count;
    
    /* distance between characters */
    dist_between_chars = 3.0 / (GLfloat)window_width();
    space_dist = 20.0 / (GLfloat)window_width();
    
    /* generate new vertices and insert into text vertex buffer */
    for(iterator = str; *iterator; ++iterator)
    {
        struct text_char_info_t* info;
        struct text_vertex_t* vertex;
        
        /* the space character requires some extra attention */
        if(wcsncmp(iterator, L" ", 1) == 0)
        {
            x_coord += space_dist;
            continue;
        }

        /* lookup character info */
        info = (struct text_char_info_t*)map_find(&font->char_map, (intptr_t)*iterator);
        if(!info)
        {
            char* buffer[sizeof(wchar_t)+1];
            memcpy(buffer, iterator, sizeof(wchar_t));
            buffer[sizeof(wchar_t)] = '\0';
            llog(LOG_ERROR, 3, "Failed to look up character: \"", buffer, "\"");
            continue;
        }

        /* top left vertex */
        vertex = (struct text_vertex_t*)unordered_vector_push_emplace(vertex_buffer);
        vertex->position[0]  = x_coord;
        vertex->position[1]  = y - info->bearing_y;
        vertex->tex_coord[0] = info->uv_left;
        vertex->tex_coord[1] = info->uv_top;
        vertex->diffuse[0]   = 1.0;
        vertex->diffuse[1]   = 1.0;
        vertex->diffuse[2]   = 1.0;
        vertex->diffuse[3]   = 1.0;
        
        /* top right vertex */
        vertex = (struct text_vertex_t*)unordered_vector_push_emplace(vertex_buffer);
        vertex->position[0]  = x_coord + info->width;
        vertex->position[1]  = y - info->bearing_y;
        vertex->tex_coord[0] = info->uv_left + info->uv_width;
        vertex->tex_coord[1] = info->uv_top;
        vertex->diffuse[0]   = 1.0;
        vertex->diffuse[1]   = 1.0;
        vertex->diffuse[2]   = 1.0;
        vertex->diffuse[3]   = 1.0;
        
        /* bottom left vertex */
        vertex = (struct text_vertex_t*)unordered_vector_push_emplace(vertex_buffer);
        vertex->position[0]  = x_coord;
        vertex->position[1]  = y - info->bearing_y - info->height;
        vertex->tex_coord[0] = info->uv_left;
        vertex->tex_coord[1] = info->uv_top + info->uv_height;
        vertex->diffuse[0]   = 1.0;
        vertex->diffuse[1]   = 1.0;
        vertex->diffuse[2]   = 1.0;
        vertex->diffuse[3]   = 1.0;
        
        /* bottom right vertex */
        vertex = (struct text_vertex_t*)unordered_vector_push_emplace(vertex_buffer);
        vertex->position[0]  = x_coord + info->width;
        vertex->position[1]  = y - info->bearing_y - info->height;
        vertex->tex_coord[0] = info->uv_left + info->uv_width;
        vertex->tex_coord[1] = info->uv_top + info->uv_height;
        vertex->diffuse[0]   = 1.0;
        vertex->diffuse[1]   = 1.0;
        vertex->diffuse[2]   = 1.0;
        vertex->diffuse[3]   = 1.0;
        
        /* generate indices */
        *(INDEX_DATA_TYPE*)unordered_vector_push_emplace(index_buffer) = base_index + 0;
        *(INDEX_DATA_TYPE*)unordered_vector_push_emplace(index_buffer) = base_index + 2;
        *(INDEX_DATA_TYPE*)unordered_vector_push_emplace(index_buffer) = base_index + 1;
        *(INDEX_DATA_TYPE*)unordered_vector_push_emplace(index_buffer) = base_index + 1;
        *(INDEX_DATA_TYPE*)unordered_vector_push_emplace(index_buffer) = base_index + 2;
        *(INDEX_DATA_TYPE*)unordered_vector_push_emplace(index_buffer) = base_index + 3;
    
        x_coord += info->width + dist_between_chars;
        base_index += 4;
    }
}

/* ------------------------------------------------------------------------- */
void
text_draw(void)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);printOpenGLError();
    glUseProgram(g_text_shader_id);printOpenGLError();
    {
        UNORDERED_VECTOR_FOR_EACH(&g_fonts, struct font_t, font)
        {
            glBindVertexArray(font->gl.vao);printOpenGLError();
                glDrawElements(GL_TRIANGLES, font->gl.static_text_num_indices, GL_UNSIGNED_SHORT, NULL);printOpenGLError();
        }
    }
    glDisable(GL_BLEND);printOpenGLError();
    glBindVertexArray(0);printOpenGLError();
}

/* ------------------------------------------------------------------------- */
/* WRAPPERS */
/* ------------------------------------------------------------------------- */
uint32_t
text_load_font_wrapper(const char* filename, uint32_t char_size)
{
    uint32_t key;
    struct font_t* font = text_load_font(filename, char_size);
    key = map_find_unused_key(&g_wrapper_fonts);
    map_insert(&g_wrapper_fonts, key, font);
    return key;
}

void
text_destroy_font_wrapper(uint32_t font_id)
{
    struct font_t* font = map_erase(&g_wrapper_fonts, font_id);
    if(font)
        text_destroy_font(font);
}

void
text_load_characters_wrapper(uint32_t font_id, const wchar_t* characters)
{
    struct font_t* font = map_find(&g_wrapper_fonts, font_id);
    if(font)
        text_load_characters(font, characters);
}

intptr_t
text_add_static_string_wrapper(uint32_t font_id, float x, float y, const wchar_t* str)
{
    intptr_t ret = -1;
    struct font_t* font = map_find(&g_wrapper_fonts, font_id);
    if(font)
        ret = text_add_static_string(font, 0, x, y, str);
    return ret;
}

intptr_t
text_add_static_center_string_wrapper(uint32_t font_id, float x, float y, const wchar_t* str)
{
    intptr_t ret = -1;
    struct font_t* font = map_find(&g_wrapper_fonts, font_id);
    if(font)
        ret = text_add_static_string(font, 1, x, y, str);
    return ret;
}

void
text_destroy_static_string_wrapper(uint32_t font_id, intptr_t ID)
{
    struct font_t* font = map_find(&g_wrapper_fonts, font_id);
    if(font)
        text_destroy_static_string(font, ID);
}

void
text_destroy_all_static_strings_wrapper(uint32_t font_id)
{
    struct font_t* font = map_find(&g_wrapper_fonts, font_id);
    if(font)
        text_destroy_all_static_strings(font);
}
