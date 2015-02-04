#include <stdio.h>
#include "plugin_renderer_gl/2d.h"
#include "plugin_renderer_gl/shader.h"
#include "plugin_renderer_gl/glutils.h"

#include "GL/glew.h"
#include "glfw3.h"

static GLuint g_line_shader_id;
static struct shapes_t* g_current_shapes = NULL;
static struct unordered_vector_t g_shapes_collection;
static uint32_t guid_counter = 1;

#ifdef _DEBUG
static const char* two_d_shader_file = "../../plugins/core/renderer_gl/fx/line_2d";
#else
static const char* two_d_shader_file = "fx/line_2d";
#endif

static struct shapes_t*
shapes_get(uint32_t ID)
{
    UNORDERED_VECTOR_FOR_EACH(&g_shapes_collection, struct shapes_t, shapes)
    {
        if(shapes->ID == ID)
            return shapes;
    }
    return NULL;
}

void
init_2d(void)
{
    g_line_shader_id = shader_load(two_d_shader_file);

    unordered_vector_init_vector(&g_shapes_collection, sizeof(struct shapes_t));
}

void
deinit_2d(void)
{
    UNORDERED_VECTOR_FOR_EACH(&g_shapes_collection, struct shapes_t, shapes)
    {
        glDeleteBuffers(1, &shapes->vbo);printOpenGLError();
        glDeleteBuffers(1, &shapes->ibo);printOpenGLError();
        glDeleteVertexArrays(1, &shapes->vao);printOpenGLError();
        unordered_vector_clear_free(&shapes->vertex_data);printOpenGLError();
        unordered_vector_clear_free(&shapes->index_data);printOpenGLError();
    }
    unordered_vector_clear_free(&g_shapes_collection);

    if(g_line_shader_id)
        glDeleteProgram(g_line_shader_id);printOpenGLError();
}

void
shapes_2d_begin(void)
{
    if(g_current_shapes)
        return;

    g_current_shapes = (struct shapes_t*)unordered_vector_push_emplace(&g_shapes_collection);
    unordered_vector_init_vector(&g_current_shapes->vertex_data, sizeof(struct vertex_2d_t));
    unordered_vector_init_vector(&g_current_shapes->index_data,  sizeof(INDEX_DATA_TYPE));
    g_current_shapes->visible = 1;
}

uint32_t
shapes_2d_end(void)
{
    uint32_t ID;

    /* current shapes must be active */
    if(!g_current_shapes)
        return 0;
    
    /* give this shapes a uinque ID */
    ID = guid_counter++;
    g_current_shapes->ID = ID;
    
    /* generate VAO, VBO, VIO, and set up render state */
    glGenVertexArrays(1, &g_current_shapes->vao);printOpenGLError();
    glBindVertexArray(g_current_shapes->vao);printOpenGLError();
        /* generate VBO for vertex data */
        glGenBuffers(1, &g_current_shapes->vbo);printOpenGLError();
        glBindBuffer(GL_ARRAY_BUFFER, g_current_shapes->vbo);
            /* copy vertex data into VBO */
            glBufferData(GL_ARRAY_BUFFER,
                        g_current_shapes->vertex_data.count * sizeof(struct vertex_2d_t),
                        g_current_shapes->vertex_data.data,
                        GL_STATIC_DRAW);printOpenGLError();
            /* set up position and diffuse vertex attributes */
            glEnableVertexAttribArray(0);printOpenGLError();
            glVertexAttribPointer(0,                      /* attribute 0 */
                                  2,                      /* size, position[2] */
                                  GL_FLOAT,               /* type */
                                  GL_FALSE,               /* normalise? */
                                  sizeof(struct vertex_2d_t),
                                  (void*)offsetof(struct vertex_2d_t, position));printOpenGLError();
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1,                      /* attribute 1 */
                                  4,                      /* diffuse[4] */
                                  GL_FLOAT,               /* type */
                                  GL_FALSE,               /* normalise? */
                                  sizeof(struct vertex_2d_t),
                                  (void*)offsetof(struct vertex_2d_t, diffuse));printOpenGLError();

        /* generate VBO for index data */
        glGenBuffers(1, &g_current_shapes->ibo);printOpenGLError();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_current_shapes->ibo);printOpenGLError();
            /* copy index data into VBO */
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                        g_current_shapes->index_data.count * sizeof(INDEX_DATA_TYPE),
                        g_current_shapes->index_data.data,
                        GL_STATIC_DRAW);printOpenGLError();
    glBindVertexArray(0);printOpenGLError();
    
    g_current_shapes = NULL;

    return ID;
}

void
shapes_2d_destroy(uint32_t ID)
{
    UNORDERED_VECTOR_FOR_EACH(&g_shapes_collection, struct shapes_t, shapes)
    {
        if(shapes->ID == ID)
        {
            glDeleteBuffers(1, &shapes->vbo);printOpenGLError();
            glDeleteBuffers(1, &shapes->ibo);printOpenGLError();
            glDeleteVertexArrays(1, &shapes->vao);printOpenGLError();
            unordered_vector_clear_free(&shapes->vertex_data);
            unordered_vector_clear_free(&shapes->index_data);
            unordered_vector_erase_element(&g_shapes_collection, shapes);
            return;
        }
    }
}

void
line_2d(float x1, float y1, float x2, float y2, uint32_t colour)
{
    struct vertex_2d_t* vertex;
    INDEX_DATA_TYPE* index;

    if(!g_current_shapes)
        return;
    
    /* add two new vertices and indices to the shapes */
    vertex = (struct vertex_2d_t*)unordered_vector_push_emplace(&g_current_shapes->vertex_data);
    vertex->position[0] = x1;
    vertex->position[1] = y1;
    vertex->diffuse[0] = (float)((colour >> 24) & 0x000000FF) / 255.0;
    vertex->diffuse[1] = (float)((colour >> 16) & 0x000000FF) / 255.0;
    vertex->diffuse[2] = (float)((colour >>  8) & 0x000000FF) / 255.0;
    vertex->diffuse[3] = (float)((colour >>  0) & 0x000000FF) / 255.0;
    index = (INDEX_DATA_TYPE*)unordered_vector_push_emplace(&g_current_shapes->index_data);
    *index = g_current_shapes->vertex_data.count - 1;

    vertex = (struct vertex_2d_t*)unordered_vector_push_emplace(&g_current_shapes->vertex_data);
    vertex->position[0] = x2;
    vertex->position[1] = y2;
    vertex->diffuse[0] = (float)((colour >> 24) & 0x000000FF) / 255.0;
    vertex->diffuse[1] = (float)((colour >> 16) & 0x000000FF) / 255.0;
    vertex->diffuse[2] = (float)((colour >>  8) & 0x000000FF) / 255.0;
    vertex->diffuse[3] = (float)((colour >>  0) & 0x000000FF) / 255.0;
    index = (INDEX_DATA_TYPE*)unordered_vector_push_emplace(&g_current_shapes->index_data);
    *index = g_current_shapes->vertex_data.count - 1;
}

void
box_2d(float x1, float y1, float x2, float y2, uint32_t colour)
{
    if(!g_current_shapes)
        return;
    
    line_2d(x1, y1, x2, y1, colour);
    line_2d(x2, y1, x2, y2, colour);
    line_2d(x2, y2, x1, y2, colour);
    line_2d(x1, y2, x1, y1, colour);
}

void
shapes_hide(uint32_t ID)
{
    struct shapes_t* shapes = shapes_get(ID);
    if(!shapes)
        return;
    shapes->visible = 0;
}

void
shapes_show(uint32_t ID)
{
    struct shapes_t* shapes = shapes_get(ID);
    if(!shapes)
        return;
    shapes->visible = 1;
}

void
draw_2d(void)
{
    glUseProgram(g_line_shader_id);printOpenGLError();
    {
        UNORDERED_VECTOR_FOR_EACH(&g_shapes_collection, struct shapes_t, shapes)
        {
            if(!shapes->visible)
                continue;

            glBindVertexArray(shapes->vao);printOpenGLError();
                glDrawElements(GL_LINES, shapes->index_data.count, GL_UNSIGNED_SHORT, NULL);printOpenGLError();
            
        }
    }
    glBindVertexArray(0);
}

SERVICE(shapes_2d_begin_wrapper)
{
    shapes_2d_begin();
}

SERVICE(shapes_2d_end_wrapper)
{
    SERVICE_RETURN(shapes_2d_end(), uint32_t);
}

SERVICE(line_2d_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, x1, float, float);
    SERVICE_EXTRACT_ARGUMENT(1, y1, float, float);
    SERVICE_EXTRACT_ARGUMENT(2, x2, float, float);
    SERVICE_EXTRACT_ARGUMENT(3, y2, float, float);
    SERVICE_EXTRACT_ARGUMENT(4, colour, uint32_t, uint32_t);

    line_2d(x1, y1, x2, y2, colour);
}

SERVICE(box_2d_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, x1, float, float);
    SERVICE_EXTRACT_ARGUMENT(1, y1, float, float);
    SERVICE_EXTRACT_ARGUMENT(2, x2, float, float);
    SERVICE_EXTRACT_ARGUMENT(3, y2, float, float);
    SERVICE_EXTRACT_ARGUMENT(4, colour, uint32_t, uint32_t);

    box_2d(x1, y1, x2, y2, colour);
}

SERVICE(shapes_2d_destroy_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, id, uint32_t, uint32_t);
    shapes_2d_destroy(id);
}

SERVICE(shapes_show_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, id, uint32_t, uint32_t);
    shapes_show(id);
}

SERVICE(shapes_hide_wrapper)
{
    SERVICE_EXTRACT_ARGUMENT(0, id, uint32_t, uint32_t);
    shapes_hide(id);
}
