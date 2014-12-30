#include "plugin_renderer_gl/2d.h"
#include "plugin_renderer_gl/shader.h"

#include "GL/glew.h"
#include "glfw3.h"

/* vertex type for 2d objects */
struct vertex_2d_t
{
    GLfloat position[3];
};

/* vao for 2d objects */
GLuint vao_2d;

/* define line */
#define LINE_2D_NUM_VERTICES 2
#define LINE_2D_NUM_INDICES 2
GLuint line_2d_vbo;
GLuint line_2d_vio;
GLuint line_2d_shader_id;
GLuint line_2d_colour_uniform_location;
GLfloat line_2d_colour[3];
static struct vertex_2d_t line_2d_vertex_data[LINE_2D_NUM_VERTICES] = {
    {-1.0, -1.0, 0.0},
    {1.0, 1.0, 0.0}
};
static GLubyte line_2d_index_data[LINE_2D_NUM_INDICES] = {
    0,
    1
};

void init_2d(void)
{
    /* setup VAO for 2d draw calls */
    glGenVertexArrays(1, &vao_2d);
    glBindVertexArray(vao_2d);
    
        /* setup vertex and index buffers for 2D lines */
        glGenBuffers(1, &line_2d_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, line_2d_vbo);
        glBufferData(GL_ARRAY_BUFFER, LINE_2D_NUM_VERTICES*sizeof(struct vertex_2d_t), line_2d_vertex_data, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,                      /* attribute 0 */
                            3,                      /* size, position[3] */
                            GL_FLOAT,               /* type */
                            GL_FALSE,               /* normalise? */
                            sizeof(struct vertex_2d_t),
                            (void*)offsetof(struct vertex_2d_t, position));
        glGenBuffers(1, &line_2d_vio);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line_2d_vio);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, LINE_2D_NUM_INDICES*sizeof(GLubyte), line_2d_index_data, GL_STATIC_DRAW);
        line_2d_shader_id = load_shader("fx/line_2d");
        if(!line_2d_shader_id)
            return;
        glUseProgram(line_2d_shader_id);
        line_2d_colour_uniform_location = glGetUniformLocation(line_2d_shader_id, "line_colour");

    /* done, unbind vao */
    glBindVertexArray(0);
}

void line(float x1, float y1, float x2, float y2)
{
    
}

void draw_2d(void)
{
    line_2d_colour[0] = 1.0;
    glBindVertexArray(vao_2d);
        glUniform3fv(line_2d_colour_uniform_location, 1, &line_2d_colour[0]);
        glDrawElements(GL_LINES, LINE_2D_NUM_INDICES, GL_UNSIGNED_BYTE, NULL);
    glBindVertexArray(0);
}