#include "plugin_renderer_gl/config.h"
#include "plugin_renderer_gl/text_manager.h"
#include "plugin_renderer_gl/text.h"
#include "plugin_renderer_gl/text_wrapper.h"
#include "plugin_renderer_gl/glutils.h"
#include "plugin_renderer_gl/shader.h"
#include "plugin_renderer_gl/window.h"
#include "util/unordered_vector.h"
#include "util/map.h"
#include "util/memory.h"
#include "util/log.h"
#include "GL/glew.h"
#include FT_BITMAP_H

/*
 * NOTE:
 *   1) We need a list of all groups to be able to render the text.
 *   2) We don't want to have to maintain two essentially duplicate lists
 *      of text group objects. Therefore, the render list is the same
 *      one being used to look up IDs for the wrapper.
 */
static struct map_t g_text_groups;
static uint32_t guid = 1;

static FT_Library g_lib;
static GLuint g_text_shader_id;

static const wchar_t* g_default_characters =
L"abcdefghijklmnopqrstuvwxyz"
L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
L"1234567890"
L" +-*/!?'^\"$%&()[]{}#@~,.";

#ifdef _DEBUG
static const char* text_shader_file = "../../plugins/core/renderer_gl/fx/text_2d";
#else
static const char* text_shader_file = "fx/text_2d";
#endif

static char
text_group_load_font(struct text_group_t* group, const char* filename, uint32_t char_size);

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
 * @brief Generates and uploads an atlass to the GPU.
 */
static void
text_group_load_atlass(struct text_group_t* group, const wchar_t* characters);

static void
text_group_sync_with_gpu(struct text_group_t* group);

/* ------------------------------------------------------------------------- */
char
text_manager_init(void)
{
    FT_Error error;
    
    /* load the text shader */
    g_text_shader_id = shader_load(text_shader_file);printOpenGLError();

    /* init freetype */
    llog(LOG_INFO, PLUGIN_NAME, 1, "Initialising freetype");
    error = FT_Init_FreeType(&g_lib);
    if(error)
    {
        llog(LOG_ERROR, PLUGIN_NAME, 1, "Failed to initialise freetype");
        return 0;
    }
    
    /* This is used as a render list and as a map to expose group objects to the
     * service API */
    map_init_map(&g_text_groups);
    
    return 1;
}

/* ------------------------------------------------------------------------- */
void
text_manager_deinit(void)
{
    /* destroy all text groups */
    /* NOTE: text_group_destroy mutates the container. Cannot iterate, so we're
     * instead abusing the fact that the map uses an ordered vector internally,
     * and know that the vector stores key-value structures.
     */
    while(g_text_groups.vector.count)
    {
        text_group_destroy(
            ((struct map_key_value_t*)ordered_vector_pop(&g_text_groups.vector))->hash
        );
    }
    map_clear_free(&g_text_groups);

    if(g_text_shader_id)
        glDeleteProgram(g_text_shader_id);printOpenGLError();

    FT_Done_FreeType(g_lib);
}

/* ------------------------------------------------------------------------- */
uint32_t
text_group_create(const char* font_filename, uint32_t char_size)
{
    struct text_group_t* group;
    uint32_t id;
    
    /* create new text group object */
    group = (struct text_group_t*)MALLOC(sizeof(struct text_group_t));
    memset(group, 0, sizeof(struct text_group_t));
    
    /* load font face */
    if(!text_group_load_font(group, font_filename, char_size))
    {
        FREE(group);
        return 0;
    }
    
    /* initialise containers */
    map_init_map(&group->char_info);
    unordered_vector_init_vector(&group->texts, sizeof(struct text_t*));
    ordered_vector_init_vector(&group->vertex_buffer, sizeof(struct text_vertex_t));
    ordered_vector_init_vector(&group->index_buffer, sizeof(INDEX_DATA_TYPE));

    /* generate VAO, VBO, VIO, and Texture buffer */
    glGenVertexArrays(1, &group->gl.vao);printOpenGLError();
    glBindVertexArray(group->gl.vao);printOpenGLError();
        glGenBuffers(1, &group->gl.vbo);printOpenGLError();
        glBindBuffer(GL_ARRAY_BUFFER, group->gl.vbo);printOpenGLError();
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
        glGenBuffers(1, &group->gl.ibo);printOpenGLError();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, group->gl.ibo);printOpenGLError();
        glGenTextures(1, &group->gl.tex);printOpenGLError();
        glBindTexture(GL_TEXTURE_2D, group->gl.tex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);printOpenGLError();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);printOpenGLError();
    glBindVertexArray(0);printOpenGLError();
    
    /* generate ID and add group to global list */
    id = guid++;
    map_insert(&g_text_groups, id, group);

    return id;
}

/* ------------------------------------------------------------------------- */
void
text_group_destroy(uint32_t id)
{
    /* remove from global list */
    struct text_group_t* group = map_erase(&g_text_groups, id);
    if(!group)
        return;
    
    /* destroy character info */
    {
        MAP_FOR_EACH(&group->char_info, struct char_info_t, key, info)
        {
            if(info)
                FREE(info);
        }
        map_clear_free(&group->char_info);
    }
    
    /* destroy texts */
    {
        UNORDERED_VECTOR_FOR_EACH(&group->texts, struct text_t*, ptext)
        {
            /* 
             * NOTE: text_destroy WILL call text_group_remove_text_object,
             * which in turn will modify this vector we're currently
             * iterating. For this not to happen, we have to first set
             * the group the text object is referencing to NULL.
             */
            (*ptext)->group = NULL;
            text_destroy(*ptext);
        }
        unordered_vector_clear_free(&group->texts);
    }
    
    /* clear vertex and index buffers */
    ordered_vector_clear_free(&group->vertex_buffer);
    ordered_vector_clear_free(&group->index_buffer);
    
    /* clean up GL stuff */
    glDeleteTextures(1, &group->gl.tex);printOpenGLError();
    glDeleteBuffers(1, &group->gl.ibo);printOpenGLError();
    glDeleteBuffers(1, &group->gl.vbo);printOpenGLError();
    glDeleteVertexArrays(1, &group->gl.vao);printOpenGLError();

    /* clean up freetype stuff */
    FT_Done_Face(group->face);
    
    /* finally, destroy font object */
    FREE(group);
}

/* ------------------------------------------------------------------------- */
struct text_group_t*
text_group_get(uint32_t id)
{
    return map_find(&g_text_groups, id);
}

/* ------------------------------------------------------------------------- */
void
text_group_load_character_set(uint32_t id, const wchar_t* characters)
{
    const wchar_t* iterator;
    const wchar_t* null_terminator = L'\0';
    struct unordered_vector_t sorted_chars;
    
    struct text_group_t* group = map_find(&g_text_groups, id);
    if(!group)
        return;
    
    /* if no characters were supplied (NULL), use default set */
    if(!characters)
        characters = g_default_characters;

    /* 
     * The following code does two things:
     *  1) Elminiate any duplicate characters.
     *     Maps will only insert elements successfully when the key is unique:
     *     Use this as a tool to filter duplicates.
     *  2) Here, NULL is being assigned to each new key. When the atlass is
     *     loaded (towards the end of this function), the NULL value will be
     *     replaced with an appropriate character info object.
     */
    for(iterator = characters; *iterator; ++iterator)
    {
        map_insert(&group->char_info, (uint32_t)*iterator, NULL);
    }

    /* 
     * Copy the characters from the map into a linear container as wchar_t's.
     * This is required to load the atlass.
     */
    unordered_vector_init_vector(&sorted_chars, sizeof(wchar_t));
    {
        MAP_FOR_EACH(&group->char_info, void, key, value)
        {
            wchar_t chr = (wchar_t)key; /* cast required before insertion due to memcpy() */
            unordered_vector_push(&sorted_chars, &chr);
        }
        unordered_vector_push(&sorted_chars, &null_terminator);
    }

    /* sorted_chars now contains all characters as wchar_t's. Ready to load atlass */
    if(sorted_chars.count > 1)
        text_group_load_atlass(group, (wchar_t*)sorted_chars.data);
    
    unordered_vector_clear_free(&sorted_chars);
}

/* ------------------------------------------------------------------------- */
void
text_group_add_text_object(struct text_group_t* text_group, struct text_t* text)
{
    UNORDERED_VECTOR_FOR_EACH(&text_group->texts, struct text_t*, pregistered_text)
    {
        if(*pregistered_text == text)
            return;
    }
    
    unordered_vector_push(&text_group->texts, &text);
    text->group = text_group;
    
    text_group->mesh_needs_reuploading = 1;
}

/* ------------------------------------------------------------------------- */
void
text_group_remove_text_object(struct text_group_t* text_group, struct text_t* text)
{
    UNORDERED_VECTOR_FOR_EACH(&text_group->texts, struct text_t*, pregistered_text)
    {
        if(*pregistered_text == text)
        {
            unordered_vector_erase_element(&text_group->texts, *pregistered_text);
            return;
        }
    }
    
    text_group->mesh_needs_reuploading = 1;
}

/* ------------------------------------------------------------------------- */
void
text_group_inform_updated_text_object(struct text_group_t* text_group)
{
    text_group->mesh_needs_reuploading = 1;
}

/* ------------------------------------------------------------------------- */
void
text_draw(void)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);printOpenGLError();
    glUseProgram(g_text_shader_id);printOpenGLError();
    {
        MAP_FOR_EACH(&g_text_groups, struct text_group_t, key, group)
        {
            /* if any text objects were updated, then mesh needs re-uploading */
            if(group->mesh_needs_reuploading)
                text_group_sync_with_gpu(group);
            
            /* render */
            glBindVertexArray(group->gl.vao);printOpenGLError();
                glDrawElements(GL_TRIANGLES, group->index_buffer.count, GL_UNSIGNED_SHORT, NULL);printOpenGLError();
            
        }
    }
    glBindVertexArray(0);printOpenGLError();
    glDisable(GL_BLEND);printOpenGLError();
}

/* ------------------------------------------------------------------------- */
static char
text_group_load_font(struct text_group_t* group, const char* filename, uint32_t char_size)
{
    FT_Error error;

    /* load face */
    error = FT_New_Face(g_lib, filename, 0, &group->face);
    if(error == FT_Err_Unknown_File_Format)
    {
        llog(LOG_ERROR, PLUGIN_NAME, 3, "The font file \"", filename, "\" could be opened and read, but it appears that its font format is unsupported");
        return 0;
    }
    else if(error)
    {
        llog(LOG_ERROR, PLUGIN_NAME, 3, "Failed to open font file \"", filename, "\"");
        return 0;
    }
    
    /* set character size */
    error = FT_Set_Char_Size(group->face, TO_26DOT6(char_size), 0, 300, 300);
    if(error)
    {
        llog(LOG_ERROR, PLUGIN_NAME, 1, "Failed to set the character size");
        return 0;
    }
    
    return 1;
}

/* ------------------------------------------------------------------------- */
static void
text_group_load_atlass(struct text_group_t* group, const wchar_t* characters)
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

        error = FT_Load_Char(group->face, *iterator, FT_LOAD_RENDER);
        if(error)
        {
            char* buffer[sizeof(wchar_t)+1];
            memcpy(buffer, iterator, sizeof(wchar_t));
            buffer[sizeof(wchar_t)] = '\0';
            llog(LOG_ERROR, PLUGIN_NAME, 3, "Failed to load glyph \"", buffer, "\"");
            continue;
        }
        
        /* calculate maximum Y bearing offset */
        offset = TO_PIXELS(group->face->glyph->metrics.horiBearingY);
        if(glyph_offset_y < offset)
            glyph_offset_y = offset;
        
        /* accumulate advances to get maximum width */
        tex_width += TO_PIXELS(group->face->glyph->advance.x) + min_advance;
        
        /* calculate maximum height */
        height = group->face->glyph->bitmap.rows;
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
        struct char_info_t* char_info;

        /* load rendered character, ignore any errors as they were already reported earlier */
        error = FT_Load_Char(group->face, *iterator, FT_LOAD_RENDER);
        if(error)
            continue;
        
        bmp_ptr = (GLubyte*)group->face->glyph->bitmap.buffer;
        bmp_width = group->face->glyph->bitmap.width;
        bmp_height = group->face->glyph->bitmap.rows;
        bmp_offset_y = glyph_offset_y - TO_PIXELS(group->face->glyph->metrics.horiBearingY);
        
        /* need to convert whatever pixel mode bitmap has to RGBA */
        switch(group->face->glyph->bitmap.pixel_mode)
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
                        /* convert single byte to R8G8B8A8, set A to 0xFF */
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
                llog(LOG_ERROR, PLUGIN_NAME, 1, "Glyph bitmap has unsupported format (conversion to RGBA needs implementing)");
                break;
        }

        /* 
         * Either create new character info object, or use an existing one if
         * the value associated with this character is not NULL (this is the
         * case if the character was already in the map)
         */
        char_info = map_find(&group->char_info, (uint32_t)*iterator);
        if(!char_info)
        {
            char_info = (struct char_info_t*)MALLOC(sizeof(struct char_info_t));
            map_set(&group->char_info, (uint32_t)*iterator, char_info);
        }

        /* save offsets as UV data */
        char_info->uv_left = (GLfloat)(pen         ) / (GLfloat)tex_width;
        char_info->uv_top  = (GLfloat)(bmp_offset_y) / (GLfloat)tex_height;
        /* save width and height in texture space */
        char_info->uv_width  = (GLfloat)bmp_width    / (GLfloat)tex_width;
        char_info->uv_height = (GLfloat)bmp_height   / (GLfloat)tex_height;
        /* save width and height in screen space */
        char_info->width     = (GLfloat)bmp_width    / (GLfloat)window_width();
        char_info->height    = (GLfloat)bmp_height   / (GLfloat)window_height();
        char_info->bearing_y = (GLfloat)bmp_offset_y / (GLfloat)window_height();

        /* advance pen */
        pen += TO_PIXELS(group->face->glyph->advance.x) + min_advance;
    }

    /*
     * Hand atlass to OpenGL and clean up.
     */
    glBindVertexArray(group->gl.vao);printOpenGLError();
        glBindTexture(GL_TEXTURE_2D, group->gl.tex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer);printOpenGLError();
    glBindVertexArray(0);printOpenGLError();
    FREE(buffer);
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
        UNORDERED_VECTOR_FOR_EACH(&group->texts, struct text_t*, ptext)
        {
            struct text_t* text = *ptext;
            if(text->visible)
            {
                uint32_t insertion_index = group->index_buffer.count;
                ordered_vector_push_vector(&group->vertex_buffer, &text->vertex_buffer);
                ordered_vector_push_vector(&group->index_buffer, &text->index_buffer);
                { ORDERED_VECTOR_FOR_EACH_RANGE(&group->index_buffer, INDEX_DATA_TYPE, val, insertion_index, group->index_buffer.count)
                {
                    (*val) += base_index;
                }}
                base_index += text->vertex_buffer.count;
            }
        }
    }

    /* upload to GPU */
    glBindVertexArray(group->gl.vao);
        glBindBuffer(GL_ARRAY_BUFFER, group->gl.vbo);
            glBufferData(GL_ARRAY_BUFFER, group->vertex_buffer.count * sizeof(struct text_vertex_t), group->vertex_buffer.data, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, group->index_buffer.count * sizeof(INDEX_DATA_TYPE), group->index_buffer.data, GL_STATIC_DRAW);
    glBindVertexArray(0);
    
    /* reset flag */
    group->mesh_needs_reuploading = 0;
}
