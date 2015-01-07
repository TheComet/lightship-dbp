#include "ft2build.h"
#include FT_FREETYPE_H
#include "util/unordered_vector.h"
#include <GL/glew.h>

#define PIXEL_FORMAT 64
#define TO_26DOT6(x) (x*PIXEL_FORMAT)
#define TO_PIXELS(x) (x/PIXEL_FORMAT)

struct text_vertex_t
{
    GLfloat position[2];
    GLfloat tex_coord[2];
    GLfloat diffuse[4];
};

struct text_glyph_atlass_t
{
    struct unordered_vector_t loaded_charcodes;
};

struct text_gl_t
{
    GLuint vao;
    GLuint vbo;
    GLuint vio;
    GLuint tex;
};

struct font_t
{
    FT_Face face;
    struct text_glyph_atlass_t atlass;
    struct text_gl_t gl;
};

char text_init(void);
void text_deinit(void);
struct font_t* text_load_font(const char* filename);
void text_destroy_font(struct font_t* font);
void text_load_characters(struct font_t* font, const wchar_t* characters);
void text_load_atlass(struct font_t* font, const wchar_t* characters);
void text_draw(void);
