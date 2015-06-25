#ifndef PLUGIN_RENDERER_GL_GLUTILS
#define PLUGIN_RENDERER_GL_GLUTILS

#ifdef _DEBUG
#   define printOpenGLError() printOglError(__FILE__, __LINE__)
#else
#   define printOpenGLError()
#endif

#define INDEX_DATA_TYPE GLushort
#include "GL/glew.h"

struct vec2_t
{
	GLfloat x;
	GLfloat y;
};

struct vertex_quad_t
{
	GLfloat position[2];
	GLfloat tex_coord[2];
};

#define VERTEX_QUAD_SETUP_ATTRIBS                                                               \
glEnableVertexAttribArray(0);printOpenGLError();                                                \
	glVertexAttribPointer(0,                /* attribute 0 */                                   \
						  2,                /* size, position[2] */                             \
						  GL_FLOAT,         /* type */                                          \
						  GL_FALSE,         /* normalise? */                                    \
						  sizeof(struct vertex_quad_t),                                         \
						  (void*)offsetof(struct vertex_quad_t, position));printOpenGLError();  \
	glEnableVertexAttribArray(1);printOpenGLError();                                            \
	glVertexAttribPointer(1,                /* attribute 1 */                                   \
						  2,                /* size, tex_coord[2] */                            \
						  GL_FLOAT,         /* type */                                          \
						  GL_FALSE,         /* normalise? */                                    \
						  sizeof(struct vertex_quad_t),                                         \
						  (void*)offsetof(struct vertex_quad_t, tex_coord));printOpenGLError();

struct vertex_2d_t
{
	GLfloat position[2];
	GLfloat diffuse[4];
};

#define VERTEX_2D_SETUP_ATTRIBS                                                              \
	glEnableVertexAttribArray(0);printOpenGLError();                                         \
	glVertexAttribPointer(0,                /* attribute 0 */                                \
						  2,                /* size, position[2] */                          \
						  GL_FLOAT,         /* type */                                       \
						  GL_FALSE,         /* normalise? */                                 \
						  sizeof(struct vertex_2d_t),                                        \
						  (void*)offsetof(struct vertex_2d_t, position));printOpenGLError(); \
	glEnableVertexAttribArray(1);                                                            \
	glVertexAttribPointer(1,                /* attribute 1 */                                \
						  4,                /* diffuse[4] */                                 \
						  GL_FLOAT,         /* type */                                       \
						  GL_FALSE,         /* normalise? */                                 \
						  sizeof(struct vertex_2d_t),                                        \
						  (void*)offsetof(struct vertex_2d_t, diffuse));printOpenGLError();

void z_error(/* should be const */char* message);
int printOglError(char *file, int line);

#endif /* PLUGIN_RENDERER_GL_GLUTILS */
