#include <stdio.h>
#include "util/bst_vector.h"
#include "util/memory.h"
#include "framework/services.h"
#include "plugin_renderer_gl/2d.h"
#include "plugin_renderer_gl/shader.h"
#include "plugin_renderer_gl/glutils.h"
#include "plugin_renderer_gl/context.h"

#include "GL/glew.h"
#include "glfw3.h"

static GLuint g_line_shader_id;
static struct shapes_t* g_current_shapes = NULL;
static struct bstv_t g_shapes_collection;
static uint32_t guid_counter = 1;

#ifdef _DEBUG
static const char* two_d_shader_file = "../../plugins/core/renderer_gl/fx/line_2d";
#else
static const char* two_d_shader_file = "fx/line_2d";
#endif

/* ------------------------------------------------------------------------- */
char
init_2d(struct context_t* context)
{
	g_line_shader_id = shader_load(context, two_d_shader_file);

	bstv_init_bstv(&g_shapes_collection);

	return 1;
}

/* ------------------------------------------------------------------------- */
void
deinit_2d(void)
{
	struct shapes_t* shapes;
	while((shapes = bstv_get_any_element(&g_shapes_collection)))
	{
		shapes_2d_destroy(shapes->id);
	}
	bstv_clear_free(&g_shapes_collection);

	if(g_line_shader_id)
		glDeleteProgram(g_line_shader_id);printOpenGLError();
}

/* ------------------------------------------------------------------------- */
void
shapes_2d_begin(struct context_t* context)
{
	if(g_current_shapes)
		return;

	/* allocate new shapes object */
	g_current_shapes = (struct shapes_t*)MALLOC(sizeof *g_current_shapes);
	if(!g_current_shapes)
		OUT_OF_MEMORY("shapes_2d_begin", RETURN_NOTHING);

	/* give this shapes a uinque ID and insert into global map */
	g_current_shapes->id = guid_counter++;
	bstv_insert(&g_shapes_collection, g_current_shapes->id, g_current_shapes);

	/* init */
	ordered_vector_init_vector(&g_current_shapes->vertex_data, sizeof(struct vertex_2d_t));
	ordered_vector_init_vector(&g_current_shapes->index_data,  sizeof(INDEX_DATA_TYPE));
	g_current_shapes->visible = 1;
}

/* ------------------------------------------------------------------------- */
uint32_t
shapes_2d_end(void)
{
	uint32_t id;

	/* current shapes must be active */
	if(!g_current_shapes)
		return 0;

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
			VERTEX_2D_SETUP_ATTRIBS
		/* generate VBO for index data */
		glGenBuffers(1, &g_current_shapes->ibo);printOpenGLError();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_current_shapes->ibo);printOpenGLError();
			/* copy index data into VBO */
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,
						g_current_shapes->index_data.count * sizeof(INDEX_DATA_TYPE),
						g_current_shapes->index_data.data,
						GL_STATIC_DRAW);printOpenGLError();
	glBindVertexArray(0);printOpenGLError();

	id = g_current_shapes->id;
	g_current_shapes = NULL;

	return id;
}

/* ------------------------------------------------------------------------- */
void
shapes_2d_destroy(uint32_t id)
{
	struct shapes_t* shapes = bstv_erase(&g_shapes_collection, id);
	if(!shapes)
		return;

	glDeleteBuffers(1, &shapes->vbo);printOpenGLError();
	glDeleteBuffers(1, &shapes->ibo);printOpenGLError();
	glDeleteVertexArrays(1, &shapes->vao);printOpenGLError();
	ordered_vector_clear_free(&shapes->vertex_data);
	ordered_vector_clear_free(&shapes->index_data);

	FREE(shapes);
}

/* ------------------------------------------------------------------------- */
void
line_2d(float x1, float y1, float x2, float y2, uint32_t colour)
{
	struct vertex_2d_t* vertex;
	INDEX_DATA_TYPE* index;

	if(!g_current_shapes)
		return;

	/* add two new vertices and indices to the shapes */
	vertex = (struct vertex_2d_t*)ordered_vector_push_emplace(&g_current_shapes->vertex_data);
	vertex->position[0] = x1;
	vertex->position[1] = y1;
	vertex->diffuse[0] = (float)((colour >> 24) & 0x000000FF) / 255.0f;
	vertex->diffuse[1] = (float)((colour >> 16) & 0x000000FF) / 255.0f;
	vertex->diffuse[2] = (float)((colour >>  8) & 0x000000FF) / 255.0f;
	vertex->diffuse[3] = (float)((colour >>  0) & 0x000000FF) / 255.0f;
	index = (INDEX_DATA_TYPE*)ordered_vector_push_emplace(&g_current_shapes->index_data);
	*index = (GLushort)g_current_shapes->vertex_data.count - 1;

	vertex = (struct vertex_2d_t*)ordered_vector_push_emplace(&g_current_shapes->vertex_data);
	vertex->position[0] = x2;
	vertex->position[1] = y2;
	vertex->diffuse[0] = (float)((colour >> 24) & 0x000000FF) / 255.0f;
	vertex->diffuse[1] = (float)((colour >> 16) & 0x000000FF) / 255.0f;
	vertex->diffuse[2] = (float)((colour >>  8) & 0x000000FF) / 255.0f;
	vertex->diffuse[3] = (float)((colour >>  0) & 0x000000FF) / 255.0f;
	index = (INDEX_DATA_TYPE*)ordered_vector_push_emplace(&g_current_shapes->index_data);
	*index = (GLushort)g_current_shapes->vertex_data.count - 1;
}

/* ------------------------------------------------------------------------- */
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

/* ------------------------------------------------------------------------- */
void
shapes_hide(uint32_t id)
{
	struct shapes_t* shapes = bstv_find(&g_shapes_collection, id);
	if(!shapes)
		return;
	shapes->visible = 0;
}

/* ------------------------------------------------------------------------- */
void
shapes_show(uint32_t id)
{
	struct shapes_t* shapes = bstv_find(&g_shapes_collection, id);
	if(!shapes)
		return;
	shapes->visible = 1;
}

/* ------------------------------------------------------------------------- */
void
draw_2d(void)
{
	glUseProgram(g_line_shader_id);printOpenGLError();

	BSTV_FOR_EACH(&g_shapes_collection, struct shapes_t, id, shapes)
		if(!shapes->visible)
			continue;

		glBindVertexArray(shapes->vao);printOpenGLError();
			glDrawElements(GL_LINES, shapes->index_data.count, GL_UNSIGNED_SHORT, NULL);printOpenGLError();
	BSTV_END_EACH

	glBindVertexArray(0);
}

/* ------------------------------------------------------------------------- */
/* WRAPPERS */
/* ------------------------------------------------------------------------- */
SERVICE(shapes_2d_begin_wrapper)
{
	struct context_t* context = get_context(service->plugin->game);
	shapes_2d_begin(context);
}

SERVICE(shapes_2d_end_wrapper)
{
	RETURN(shapes_2d_end(), uint32_t);
}

SERVICE(line_2d_wrapper)
{
	EXTRACT_ARGUMENT(0, x1, float, float);
	EXTRACT_ARGUMENT(1, y1, float, float);
	EXTRACT_ARGUMENT(2, x2, float, float);
	EXTRACT_ARGUMENT(3, y2, float, float);
	EXTRACT_ARGUMENT(4, colour, uint32_t, uint32_t);

	line_2d(x1, y1, x2, y2, colour);
}

SERVICE(box_2d_wrapper)
{
	EXTRACT_ARGUMENT(0, x1, float, float);
	EXTRACT_ARGUMENT(1, y1, float, float);
	EXTRACT_ARGUMENT(2, x2, float, float);
	EXTRACT_ARGUMENT(3, y2, float, float);
	EXTRACT_ARGUMENT(4, colour, uint32_t, uint32_t);

	box_2d(x1, y1, x2, y2, colour);
}

SERVICE(shapes_2d_destroy_wrapper)
{
	EXTRACT_ARGUMENT(0, id, uint32_t, uint32_t);
	shapes_2d_destroy(id);
}

SERVICE(shapes_show_wrapper)
{
	EXTRACT_ARGUMENT(0, id, uint32_t, uint32_t);
	shapes_show(id);
}

SERVICE(shapes_hide_wrapper)
{
	EXTRACT_ARGUMENT(0, id, uint32_t, uint32_t);
	shapes_hide(id);
}
