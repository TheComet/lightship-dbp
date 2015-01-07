#include "plugin_renderer_gl/text.h"
#include "plugin_renderer_gl/shader.h"
#include "util/log.h"
#include "util/memory.h"
#include "util/unordered_vector.h"
#include "util/string.h"
#include "glfw3.h"
#include <wchar.h>
#include <math.h>

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
    
    static struct text_vertex_t vertices[4] = {
        {{-1,-1}, {0, 0}, {1, 1, 1, 1}},
        {{-1, 1}, {0, 1}, {1, 1, 1, 1}},
        {{ 1,-1}, {1, 0}, {1, 1, 1, 1}},
        {{ 1, 1}, {1, 1}, {1, 1, 1, 1}}
    };
    
    static GLushort indices[6] = {
        0, 2, 3,
        0, 3, 1
    };
    
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
        
        /* generate VAO, VBO, VIO, and Texture buffer */
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
            glGenTextures(1, &font->gl.tex);
            glBindTexture(GL_TEXTURE_2D, font->gl.tex);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    
    /* with the list of characters, (re)load atlass *
    if(font->atlass.loaded_charcodes.count > 1)
        text_load_atlass(font, (wchar_t*)font->atlass.loaded_charcodes.data);*/
}

void text_load_atlass(struct font_t* font, const wchar_t* characters)
{
    const wchar_t* iterator;
    FT_Error error;
    unsigned int num_characters_sqrt;
    unsigned int tex_size;
    unsigned int max_glyph_size = 0;
    
    /*
     * First, find the glyph with the maximum width and the glyph with the
     * maximum height. This determines the horizontal spacing of the glyphs
     * on the atlass.
     * Report any missing glyphs to the log.
     */
    for(iterator = characters; *iterator; ++iterator)
    {
        error = FT_Load_Glyph(font->face,
                              FT_Get_Char_Index(font->face, *iterator),
                              FT_LOAD_NO_BITMAP);
        if(error)
        {
            char* buffer[sizeof(wchar_t)+1];
            memcpy(buffer, iterator, sizeof(wchar_t));
            buffer[sizeof(wchar_t)] = '\0';
            llog(LOG_ERROR, 3, "Failed to load glyph \"", buffer, "\"");
            continue;
        }
        
        if(font->face->glyph->metrics.width > max_glyph_size)
            max_glyph_size = font->face->glyph->metrics.width;
        if(font->face->glyph->metrics.height > max_glyph_size)
            max_glyph_size = font->face->glyph->metrics.height;
    }
    
    max_glyph_size = TO_PIXELS(max_glyph_size);
    
    /*
     * With the maximum width determined, calculate the width and height of the
     * target texture, then generate it.
     */
    num_characters_sqrt = (unsigned int)ceil(sqrt((double)wcslen(characters)));
    tex_size = 32;
    while(num_characters_sqrt * max_glyph_size < tex_size)
        tex_size <<= 1;
    glBindVertexArray(font->gl.vao);
    {
        unsigned int current_x = 0;
        unsigned int current_y = 0;

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGB,
                     tex_size,
                     tex_size,
                     0,
                     GL_BGR,
                     GL_UNSIGNED_BYTE,
                     NULL);
        
        /* copy all glyphs into texture */
        for(iterator = characters; *iterator; ++iterator)
        {
            /* 
             * Load glyph into slot, ignore errors as they were already
             * reported in the first pass
             */
            error = FT_Load_Glyph(font->face,
                              FT_Get_Char_Index(font->face, *iterator),
                              FT_LOAD_DEFAULT);
            if(error)
                continue;
            
            /* if no bitmap was loaded, do so now */
            if(font->face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
            {
                error = FT_Render_Glyph(font->face->glyph, FT_RENDER_MODE_NORMAL);
                if(error)
                {
                    char* buffer[sizeof(wchar_t)+1];
                    memcpy(buffer, iterator, sizeof(wchar_t));
                    buffer[sizeof(wchar_t)] = '\0';
                    llog(LOG_ERROR, 3, "Failed to render glyph \"", buffer, "\"");
                    continue;
                }
            }
            
            /* copy into openGL texture */
            glTexSubImage2D(GL_TEXTURE_2D,
                            0,
                            current_x,
                            current_y,
                            font->face->glyph->metrics.width,
                            font->face->glyph->metrics.height,
                            GL_BGR,
                            GL_UNSIGNED_BYTE,
                            font->face->glyph->bitmap.buffer);
        }
    }
    glBindVertexArray(0);
}

void text_draw(void)
{
    glUseProgram(g_text_shader_id);
    {
        UNORDERED_VECTOR_FOR_EACH(&g_fonts, struct font_t, font)
        {
            glBindVertexArray(font->gl.vao);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
        }
    }
    glBindVertexArray(0);
}
