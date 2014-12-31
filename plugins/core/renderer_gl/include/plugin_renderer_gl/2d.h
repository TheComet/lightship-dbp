#include "GL/glew.h"
#include "util/unordered_vector.h"


/* vertex type for 2d objects */
struct vertex_2d_t
{
    GLfloat position[2];
    GLfloat diffuse[4];
};

struct batch_t
{
    GLuint vbo;
    struct unordered_vector_t vertex_data;
};

/*!
 * @brief Initialises 2D sub-system. Call this before calling any other 2D 
 * related calls.
 */
void init_2d(void);

/*!
 * @brief De-inits 2D subsystem. Cleans up all buffers.
 */
void deinit_2d(void);

/*!
 * @brief Begin drawing a batch of shapes.
 */
void batch_2d_begin(void);

/*!
 * @brief End drawing a batch of shapes.
 */
void batch_2d_end(void);

/*!
 * @brief Adds a line to the batch.
 * @note Call batch_2d_begin() to start a batch.
 * @param x1 Starting X coordinate of line in GL screen space.
 * @param y1 Starting Y coordinate of line in GL screen space.
 * @param x2 Ending X coordinate of line in GL screen space.
 * @param y2 Ending X coordinate of line in GL screen space.
 */
void line(float x1, float y1, float x2, float y2, uint32_t colour1, uint32_t colour2);

/*!
 * @brief Draws all batches.
 */
void draw_2d(void);
