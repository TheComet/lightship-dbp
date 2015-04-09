#include "GL/glew.h"
#include "util/unordered_vector.h"
#include "framework/service_api.h"

struct glob_t;

struct shapes_t
{
    uint32_t ID;
    uint8_t visible;
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    struct unordered_vector_t vertex_data;
    struct unordered_vector_t index_data;
};

/*!
 * @brief Initialises 2D sub-system. Call this before calling any other 2D 
 * related calls.
 */
char
init_2d(struct glob_t* g);

/*!
 * @brief De-inits 2D subsystem. Cleans up all buffers.
 */
void
deinit_2d(void);

/*!
 * @brief Begin drawing a collection of shapes.
 */
void
shapes_2d_begin(void);

/*!
 * @brief End drawing a collection of shapes.
 * @return Returns a unique ID for this shape. This can be used to reference it
 * later.
 */
uint32_t
shapes_2d_end(void);

void
shapes_2d_destroy(uint32_t ID);

/*!
 * @brief Hides the specified shape of shapes.
 * @param ID The unique ID of the shape to hide.
 */
void
shapes_hide(uint32_t ID);

/*!
 * @brief Shows the specified shape of shapes.
 * @param ID The unique ID of the shape to show.
 */
void
shapes_show(uint32_t ID);

/*!
 * @brief Adds a line to the shape.
 * @note Call shape_2d_begin() to start a shape.
 * @param x1 Starting X coordinate of line in GL screen space.
 * @param y1 Starting Y coordinate of line in GL screen space.
 * @param x2 Ending X coordinate of line in GL screen space.
 * @param y2 Ending X coordinate of line in GL screen space.
 */
void
line_2d(float x1, float y1, float x2, float y2, uint32_t colour);

void
box_2d(float x1, float y1, float x2, float y2, uint32_t colour);

/*!
 * @brief Draws all shapes.
 */
void
draw_2d(void);

SERVICE(shapes_2d_begin_wrapper);
SERVICE(shapes_2d_end_wrapper);
SERVICE(line_2d_wrapper);
SERVICE(box_2d_wrapper);
SERVICE(shapes_2d_destroy_wrapper);
SERVICE(shapes_show_wrapper);
SERVICE(shapes_hide_wrapper);
