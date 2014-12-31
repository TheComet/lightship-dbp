#include "plugin_renderer_gl/2d.h"
#include "plugin_renderer_gl/shader.h"

#include "GL/glew.h"
#include "glfw3.h"

GLuint g_vao_2d; /* vao for 2d objects */
GLuint g_line_shader_id;
struct batch_t* g_current_batch = NULL;
struct unordered_vector_t g_batches;

void init_2d(void)
{
    /* setup VAO for 2d draw calls */
    glGenVertexArrays(1, &g_vao_2d);
    
    /* load shaders */
    g_line_shader_id = load_shader("fx/line_2d");
    
    unordered_vector_init_vector(&g_batches, sizeof(struct batch_t));
}

void deinit_2d(void)
{
    UNORDERED_VECTOR_FOR_EACH(&g_batches, struct batch_t, batch)
    {
        unordered_vector_clear_free(&batch->vertex_data);
    }
    unordered_vector_clear_free(&g_batches);
}

void batch_2d_begin(void)
{
    if(g_current_batch)
        return;

    g_current_batch = (struct batch_t*)unordered_vector_push_emplace(&g_batches);
    unordered_vector_init_vector(&g_current_batch->vertex_data, sizeof(struct vertex_2d_t));
}

void line(float x1, float y1, float x2, float y2, uint32_t colour1, uint32_t colour2)
{
    struct vertex_2d_t* vertex;
    if(!g_current_batch)
        return;
    
    /* add two new vertices to the batch */
    vertex = (struct vertex_2d_t*)unordered_vector_push_emplace(&g_current_batch->vertex_data);
    vertex->position[0] = x1;
    vertex->position[1] = y1;
    vertex->diffuse[0] = (float)((colour1 >> 24) & 0x000000FF) / 255.0;
    vertex->diffuse[1] = (float)((colour1 >> 16) & 0x000000FF) / 255.0;
    vertex->diffuse[2] = (float)((colour1 >>  8) & 0x000000FF) / 255.0;
    vertex->diffuse[3] = (float)((colour1 >>  0) & 0x000000FF) / 255.0;
    vertex = (struct vertex_2d_t*)unordered_vector_push_emplace(&g_current_batch->vertex_data);
    vertex->position[0] = x2;
    vertex->position[1] = y2;
    vertex->diffuse[0] = (float)((colour2 >> 24) & 0x000000FF) / 255.0;
    vertex->diffuse[1] = (float)((colour2 >> 16) & 0x000000FF) / 255.0;
    vertex->diffuse[2] = (float)((colour2 >>  8) & 0x000000FF) / 255.0;
    vertex->diffuse[3] = (float)((colour2 >>  0) & 0x000000FF) / 255.0;
}

void batch_2d_end(void)
{
    if(!g_current_batch)
        return;
    
    glBindVertexArray(g_vao_2d);
        glGenBuffers(1, &g_current_batch->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, g_current_batch->vbo);
            glBufferData(GL_ARRAY_BUFFER,
                         g_current_batch->vertex_data.count * sizeof(struct vertex_2d_t),
                         g_current_batch->vertex_data.data,
                         GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0,                      /* attribute 0 */
                                  2,                      /* size, position[2] */
                                  GL_FLOAT,               /* type */
                                  GL_FALSE,               /* normalise? */
                                  sizeof(struct vertex_2d_t),
                                  (void*)offsetof(struct vertex_2d_t, position));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1,                      /* attribute 1 */
                                  4,                      /* diffuse[4] */
                                  GL_FLOAT,               /* type */
                                  GL_FALSE,               /* normalise? */
                                  sizeof(struct vertex_2d_t),
                                  (void*)offsetof(struct vertex_2d_t, diffuse));
        glUseProgram(g_line_shader_id);
    
    g_current_batch = NULL;
}

void draw_2d(void)
{
    glBindVertexArray(g_vao_2d);
    {
        UNORDERED_VECTOR_FOR_EACH(&g_batches, struct batch_t, batch)
        {
            glBindBuffer(GL_ARRAY_BUFFER, batch->vbo);
                glDrawArrays(GL_LINES, 0, batch->vertex_data.count);
        }
    }
}
