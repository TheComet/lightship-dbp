#include "ft2build.h"
#include FT_FREETYPE_H
#include "util/unordered_vector.h"
#include "util/map.h"
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

struct text_gl_t
{
    GLuint vao;
    GLuint vbo;
    GLuint vio;
    GLuint tex;
    GLuint static_text_num_indices;
};

struct text_char_info_t
{
    GLfloat uv_top;
    GLfloat uv_left;
    GLfloat uv_width;
    GLfloat uv_height;
};

struct font_t
{
    FT_Face face;
    struct text_gl_t gl;
    struct map_t char_map;  /* maps character codes to instances of text_char_info_t */
    struct map_t static_text_map;
};

/*!
 * @brief Initialises the text subsystem. Call this before calling any other
 * related text functions.
 */
char
text_init(void);

/*!
 * @brief De-initialises the text subsystem, cleaning up memory.
 */
void
text_deinit(void);

/*!
 * @brief Loads a new font from a font file.
 * @param[in] filename The font file to load.
 * @return A new font object which can be used for later text related calls.
 */
struct font_t*
text_load_font(const char* filename);

/*!
 * @brief Destroys and unloads the specified font.
 * @param[in] font The font object to destroy.
 */
void
text_destroy_font(struct font_t* font);

/*!
 * @brief Loads all characters specified.
 * 
 * This needs to be called before being able to draw text. The specified
 * characters specified will be the only characters that can be drawn to the
 * screen.
 * @note Calling this multiple times for the same font is fine, in case you
 * need to adjust the set of characters.
 * @param[in] font The font object to load characters for.
 * @param[in] characters The characters to load.
 * @note Use wide characters by specifying an "L", e.g. L"abcde..."
 */
void
text_load_characters(struct font_t* font, const wchar_t* characters);

/*!
 * @brief Generates and uploads an atlass to the GPU.
 * @note Internal function.
 */
void
text_load_atlass(struct font_t* font, const wchar_t* characters);

void
text_add_static(struct font_t* font, GLfloat x, GLfloat y, const wchar_t* str);

/*!
 * @brief The draw call. Draws all existing text to the screen.
 */
void
text_draw(void);
