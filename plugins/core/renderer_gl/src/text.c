#include "plugin_renderer_gl/text.h"
#include "plugin_renderer_gl/shader.h"
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
#else
static const char* ttf_prefix = "./";
#endif

int z_verbose = 0;
void z_error(/* should be const */char* message)
{
    llog(LOG_ERROR, 1, message);
}

#define printOpenGLError() printOglError(__FILE__, __LINE__)

int printOglError(char *file, int line)
{

    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n",
                 file, line, gluErrorString(glErr));
        retCode = 1;
    }
    return retCode;
}

static GLuint to_nearest_pow2(GLuint value)
{
    GLuint nearest = 2;
    while((nearest <<= 1) < value);
    return nearest;
}

char text_init(void)
{
    FT_Error error;

    /* init freetype */
    error = FT_Init_FreeType(&g_lib);
    if(error)
    {
        llog(LOG_ERROR, 1, "Failed to initialise freetype");
        return 0;
    }
    
    g_text_shader_id = load_shader("fx/text_2d");
    
    /* init global vector for storing all loaded fonts */
    unordered_vector_init_vector(&g_fonts, sizeof(struct font_t));
    
    return 1;
}

void text_deinit(void)
{
    while(g_fonts.count)
        text_destroy_font((struct font_t*)g_fonts.data);
    unordered_vector_clear_free(&g_fonts);

    glDeleteProgram(g_text_shader_id);

    FT_Done_FreeType(g_lib);
}

struct font_t* text_load_font(const char* filename)
{
    FT_Error error;
    char* ttf_filename;
    struct font_t* font;
    
    /* create new font object */
    font = (struct font_t*)unordered_vector_push_emplace(&g_fonts);
    unordered_vector_init_vector(&font->atlass.loaded_charcodes, sizeof(wchar_t));
    
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
        
        static struct text_vertex_t vertices[4] = {
            {{-1,-1}, {0,0}, {1,1,1,1}},
            {{-1, 1}, {0,1}, {1,1,1,1}},
            {{ 1,-1}, {1,0}, {1,1,1,1}},
            {{ 1, 1}, {1,1}, {1,1,1,1}}
        };
        
        static GLushort indices[6] = {
            0, 3, 1,
            0, 2, 3
        };
        
        /* generate VAO, VBO, VIO, and Texture buffer */
        printOpenGLError();
        glGenVertexArrays(1, &font->gl.vao);
        glBindVertexArray(font->gl.vao);
            glGenBuffers(1, &font->gl.vbo);
            glBindBuffer(GL_ARRAY_BUFFER, font->gl.vbo);
                glBufferData(GL_ARRAY_BUFFER, 4*sizeof(struct text_vertex_t), vertices, GL_STATIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0,                /* attribute 0 */
                                      2,                /* size, position[2] */
                                      GL_FLOAT,         /* type */
                                      GL_FALSE,         /* normalise? */
                                      sizeof(struct text_vertex_t),
                                      (void*)offsetof(struct text_vertex_t, position));
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1,                /* attribute 1 */
                                      2,                /* size, tex_coord[2] */
                                      GL_FLOAT,         /* type */
                                      GL_FALSE,         /* normalise? */
                                      sizeof(struct text_vertex_t),
                                      (void*)offsetof(struct text_vertex_t, tex_coord));
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2,                /* attribute 2 */
                                      4,                /* size, diffuse[4] */
                                      GL_FLOAT,         /* type */
                                      GL_FALSE,         /* normalise? */
                                      sizeof(struct text_vertex_t),
                                      (void*)offsetof(struct text_vertex_t, diffuse));
            glGenBuffers(1, &font->gl.vio);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, font->gl.vio);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(GLushort), indices, GL_STATIC_DRAW);
            glGenTextures(1, &font->gl.tex);printOpenGLError();
            glBindTexture(GL_TEXTURE_2D, font->gl.tex);printOpenGLError();
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);printOpenGLError();
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);printOpenGLError();
        glBindVertexArray(0);
        
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

void text_destroy_font(struct font_t* font)
{
    glDeleteTextures(1, &font->gl.tex);
    glDeleteBuffers(1, &font->gl.vio);
    glDeleteBuffers(1, &font->gl.vbo);
    glDeleteVertexArrays(1, &font->gl.vao);
    FT_Done_Face(font->face);
    unordered_vector_clear_free(&font->atlass.loaded_charcodes);
    unordered_vector_erase_element(&g_fonts, font);
}

void text_load_characters(struct font_t* font, const wchar_t* characters)
{
    const wchar_t* iterator;
    wchar_t* last_element;
    wchar_t null_terminator = L'\0';

    /* get a pointer to where the new characters can be copied to (last element in vector) */
    if(!font->atlass.loaded_charcodes.data)
        unordered_vector_push(&font->atlass.loaded_charcodes, &null_terminator);
    last_element = ((wchar_t*)font->atlass.loaded_charcodes.data) + font->atlass.loaded_charcodes.count - 1;

    for(iterator = characters; *iterator; ++iterator)
    {
        /* don't bother adding characters already loaded */
        if(font->atlass.loaded_charcodes.data)
            if(wcschr((wchar_t*)font->atlass.loaded_charcodes.data, *iterator))
                continue;

        /* add character and shift pointer to point at the new end */
        *last_element = *iterator;
        last_element = (wchar_t*)unordered_vector_push_emplace(&font->atlass.loaded_charcodes);
        *last_element = null_terminator;
    }
    
    /* with the list of characters, (re)load atlass */
    if(font->atlass.loaded_charcodes.count > 1)
        text_load_atlass(font, (wchar_t*)font->atlass.loaded_charcodes.data);
}

void text_load_atlass(struct font_t* font, const wchar_t* characters)
{
    const wchar_t* iterator;
    FT_Error error;
    unsigned int pen;
    unsigned int tex_width, tex_height;
    GLuint* buffer = NULL;
    
    /*
     * First, find the glyph with the maximum height and accumulate all advance
     * spacings. This determines the dimensions of the atlass.
     * Report any missing glyphs to the log.
     */
    tex_width = 0;
    tex_height = 0;
    for(iterator = characters; *iterator; ++iterator)
    {
        error = FT_Load_Char(font->face, *iterator, FT_LOAD_RENDER);
        if(error)
        {
            char* buffer[sizeof(wchar_t)+1];
            memcpy(buffer, iterator, sizeof(wchar_t));
            buffer[sizeof(wchar_t)] = '\0';
            llog(LOG_ERROR, 3, "Failed to load glyph \"", buffer, "\"");
            continue;
        }
        
        if(tex_height < font->face->glyph->bitmap.rows)
            tex_height = font->face->glyph->bitmap.rows;
        tex_width += TO_PIXELS(font->face->glyph->advance.x);
    }

    /*
     * With the maximum width and height determined, calculate the width and
     * height of the target texture atlass.
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

        /* load rendered character, ignore any errors as they were already reported earlier */
        error = FT_Load_Char(font->face, *iterator, FT_LOAD_RENDER);
        if(error)
            continue;
        
        /* need to convert whatever pixel mode bitmap has to RGBA */
        switch(font->face->glyph->bitmap.pixel_mode)
        {
            GLubyte* bmp_ptr;
            unsigned int bmp_width, bmp_height, x, y;
    
            /* 8-bit pixel modes */
            case FT_PIXEL_MODE_GRAY:
            case FT_PIXEL_MODE_LCD:
            case FT_PIXEL_MODE_LCD_V:
                bmp_ptr = (GLubyte*)font->face->glyph->bitmap.buffer;
                bmp_width = font->face->glyph->bitmap.width;
                bmp_height = font->face->glyph->bitmap.rows;
                for(y = 0; y != bmp_height; ++y)
                {
                    GLuint* buffer_ptr = buffer + y*tex_width + pen;
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

        pen += TO_PIXELS(font->face->glyph->advance.x);
    }
    
    glBindVertexArray(font->gl.vao);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer);
    glBindVertexArray(0);
    FREE(buffer);
}

void text_draw(void)
{
    glUseProgram(g_text_shader_id);
    {
        UNORDERED_VECTOR_FOR_EACH(&g_fonts, struct font_t, font)
        {
            glBindVertexArray(font->gl.vao);printOpenGLError();
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
        }
    }
    glBindVertexArray(0);
}
