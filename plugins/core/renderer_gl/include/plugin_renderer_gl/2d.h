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

void init_2d(void);
void deinit_2d(void);
void batch_2d_begin(void);
void batch_2d_end(void);
void line(float x1, float y1, float x2, float y2, uint32_t colour1, uint32_t colour2);
void draw_2d(void);
