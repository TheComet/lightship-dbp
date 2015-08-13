#include <GL/glew.h>

struct context_t;

GLuint
shader_load(struct context_t* g, const char* name);

GLuint
load_shader_pair(struct context_t* g, const char* vertex_shader, const char* fragment_shader);
