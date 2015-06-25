/*!
 * @file font.h
 * @brief Loading and drawing of text with different fonts.
 */

#include "util/ordered_vector.h"
#include "util/unordered_vector.h"
#include "util/map.h"
#include <GL/glew.h>
#include "ft2build.h"
#include FT_FREETYPE_H

#define PIXEL_FORMAT 64
#define TO_26DOT6(x) (x*PIXEL_FORMAT)
#define TO_PIXELS(x) (x/PIXEL_FORMAT)

struct glob_t;
struct text_t;

struct text_gl_t
{
	GLuint vao;
	GLuint vbo;
	GLuint ibo;
	GLuint tex;
};

struct char_info_t
{
	GLfloat uv_top;
	GLfloat uv_left;
	GLfloat uv_width;
	GLfloat uv_height;
	GLfloat width;
	GLfloat height;
	GLfloat bearing_y;
};

struct text_group_t
{
	FT_Face face;
	struct text_gl_t gl;
	/* maps character codes to instances of text_manager_char_info_t */
	struct map_t char_info;
	/* maps text IDs to text_t instances */
	struct unordered_vector_t texts;
	/* keep text type specific vertex and index buffers alive for the entirity of the font's life,
	 * to cut back on allocation frequency whenever all text objects are accumulated and
	 * uploaded to the GPU */
	struct ordered_vector_t vertex_buffer;
	struct ordered_vector_t index_buffer;
	char mesh_needs_reuploading;
};

char
text_manager_init(struct glob_t* g);

void
text_manager_deinit(void);

/*!
 * @brief Loads a new font from a font file.
 * @param[in] filename The font file to load.
 * @return A new font object which can be used for later text related calls.
 */
uint32_t
text_group_create(struct glob_t* g,
				  const char* font_filename,
				  uint32_t char_size);

/*!
 * @brief Destroys and unloads the specified font.
 * @param[in] font The font object to destroy.
 */
void
text_group_destroy(uint32_t id);

struct text_group_t*
text_group_get(uint32_t id);

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
 * @note Use NULL to load the default set of characters.
 */
void
text_group_load_character_set(struct glob_t* g,
							  uint32_t id,
							  const wchar_t* characters);

void
text_group_add_text_object(struct text_group_t* text_group, struct text_t* text);

void
text_group_remove_text_object(struct text_group_t* text_group, struct text_t* text);

void
text_group_inform_updated_text_object(struct text_group_t* text_group);

/*!
 * @brief The draw call. Draws all text from all text groups to the screen.
 */
void
text_draw(void);
