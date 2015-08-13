#include "plugin_renderer_gl/config.h"
#include "plugin_renderer_gl/shader.h"
#include "plugin_renderer_gl/context.h"
#include "framework/log.h"
#include "util/file.h"
#include "util/memory.h"
#include "util/string.h"
#include <stdio.h>
#include <string.h>

#include "GL/glew.h"

/* ------------------------------------------------------------------------- */
static char
check_shader(struct context_t* context, GLuint shader_ID)
{
	GLint result = GL_FALSE;
	int info_log_length;
	char* message = NULL;

	glGetShaderiv(shader_ID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader_ID, GL_INFO_LOG_LENGTH, &info_log_length);
	message = (char*)MALLOC(info_log_length);
	glGetShaderInfoLog(shader_ID, info_log_length, NULL, message);
	if(result == GL_FALSE)
		llog(LOG_ERROR, context->game, PLUGIN_NAME, message);
	FREE(message);

	return (result != GL_FALSE);
}

/* ------------------------------------------------------------------------- */
static char*
load_and_compile_shader(struct context_t* context, GLuint shader_ID, const char* file_name)
{
	GLchar* code;

	/* copy file into memory */
	file_load_into_memory(file_name, (void**)&code, 0);
	if(!code)
	{
		llog(LOG_ERROR, context->game, PLUGIN_NAME, "failed to load file \"%s\"", file_name);
		return NULL;
	}

	/* compile */
	llog(LOG_INFO, context->game, PLUGIN_NAME, "compiling shader: \"%s\"", file_name);
	glShaderSource(shader_ID, 1, (const GLchar**)&code, NULL);
	glCompileShader(shader_ID);

	return code;
}

/* ------------------------------------------------------------------------- */
static char
check_program(struct context_t* context, GLuint program_ID)
{
	GLint result = GL_FALSE;
	int info_log_length;
	char* message = NULL;

	glGetProgramiv(program_ID, GL_LINK_STATUS, &result);
	glGetProgramiv(program_ID, GL_INFO_LOG_LENGTH, &info_log_length);
	message = (char*)MALLOC(info_log_length);
	glGetProgramInfoLog(program_ID, info_log_length, NULL, message);
	if(result == GL_FALSE)
		llog(LOG_ERROR, context->game, PLUGIN_NAME, message);
	FREE(message);

	return (result != GL_FALSE);
}

/* ------------------------------------------------------------------------- */
GLuint
shader_load(struct context_t* context, const char* name)
{
	char* vertex_shader;
	char* fragment_shader;
	GLuint result;

	vertex_shader = cat_strings(2, name, ".vsh");
	fragment_shader = cat_strings(2, name, ".fsh");
	result = load_shader_pair(context, vertex_shader, fragment_shader);

	free_string(vertex_shader);
	free_string(fragment_shader);
	return result;
}

/* ------------------------------------------------------------------------- */
GLuint
load_shader_pair(struct context_t* context,
				 const char* vertex_shader,
				 const char* fragment_shader)
{
	GLuint program_ID;
	GLuint vsh_ID;
	GLuint fsh_ID;
	char* vsh_code = NULL;
	char* fsh_code = NULL;

	/* compile shaders */
	vsh_ID = glCreateShader(GL_VERTEX_SHADER);
	fsh_ID = glCreateShader(GL_FRAGMENT_SHADER);
	vsh_code = load_and_compile_shader(context, vsh_ID, vertex_shader);
	check_shader(context, vsh_ID);
	fsh_code = load_and_compile_shader(context, fsh_ID, fragment_shader);
	check_shader(context, fsh_ID);

	/* link program */
	llog(LOG_INFO, context->game, PLUGIN_NAME, "linking program");
	program_ID = glCreateProgram();
	glAttachShader(program_ID, vsh_ID);
	glAttachShader(program_ID, fsh_ID);
	glLinkProgram(program_ID);
	if(!check_program(context, program_ID))
	{
		if(vsh_code)
			llog(LOG_ERROR, context->game, NULL,
				"================= Vertex Shader Dump =================\n%s",
				vsh_code
			);
		if(fsh_code)
			llog(LOG_ERROR, context->game, NULL,
				"================= Fragment Shader Dump =================\n%s",
				fsh_code
			);
	}

	if(vsh_code)
		free_file(vsh_code);
	if(fsh_code)
		free_file(fsh_code);
	glDeleteShader(vsh_ID);
	glDeleteShader(fsh_ID);

	return program_ID;
}
