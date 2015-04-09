#include <GL/glew.h>

struct glob_t;

GLuint
shader_load(struct glob_t* g, const char* name);

GLuint
load_shader_pair(struct glob_t* g, const char* vertex_shader, const char* fragment_shader);
