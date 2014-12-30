#include "plugin_renderer_gl/shader.h"
#include "util/memory.h"
#include "util/log.h"
#include "util/string.h"
#include <stdio.h>

#include "GL/glew.h"

void check_shader(GLuint shader_ID)
{
    GLint result = GL_FALSE;
    int info_log_length;
    char* message = NULL;

    glGetShaderiv(shader_ID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader_ID, GL_INFO_LOG_LENGTH, &info_log_length);
    message = (char*)MALLOC(info_log_length);
    glGetShaderInfoLog(shader_ID, info_log_length, NULL, message);
    if(result == GL_FALSE)
        llog(LOG_ERROR, 1, message);
    else
        llog(LOG_INFO, 1, message);
    FREE(message);
}

void compile_shader(GLuint shader_ID, const char* file_name)
{
    FILE* fp;
    GLchar* code;
    int code_num_bytes;

    /* copy file into memory */
    fp = fopen(file_name, "r");
    if(!fp)
    {
        llog(LOG_ERROR, 3, "failed to open file \"", file_name, "\"");
        return;
    }
    fseek(fp, 0, SEEK_END);
    code_num_bytes = ftell(fp);
    rewind(fp);
    code = (GLchar*)MALLOC(code_num_bytes + 1);
    fread(code, sizeof(char), code_num_bytes, fp);
    code[code_num_bytes] = '\0';
    fclose(fp);

    /* compile */
    llog(LOG_INFO, 3, "compiling shader: \"", file_name, "\"");
    glShaderSource(shader_ID, 1, (const GLchar**)&code, NULL);
    glCompileShader(shader_ID);
    
    FREE(code);
}

void check_program(GLuint program_ID)
{
    GLint result = GL_FALSE;
    int info_log_length;
    char* message = NULL;
    
    glGetProgramiv(program_ID, GL_LINK_STATUS, &result);
    glGetProgramiv(program_ID, GL_INFO_LOG_LENGTH, &info_log_length);
    message = (char*)MALLOC(info_log_length);
    glGetProgramInfoLog(program_ID, info_log_length, NULL, message);
    if(result == GL_FALSE)
        llog(LOG_ERROR, 1, message);
    else
        llog(LOG_INFO, 1, message);
    FREE(message);
}

GLuint load_shader(const char* name)
{
    char* vertex_shader;
    char* fragment_shader;
    GLuint result;

    vertex_shader = cat_strings(2, name, ".vsh");
    fragment_shader = cat_strings(2, name, ".fsh");
    result = load_shader_pair(vertex_shader, fragment_shader);
    
    FREE(vertex_shader);
    FREE(fragment_shader);
    return result;
}

GLuint load_shader_pair(const char* vertex_shader, const char* fragment_shader)
{
    GLuint program_ID;
    GLuint vsh_ID;
    GLuint fsh_ID;
    
    /* compile shaders */
    vsh_ID = glCreateShader(GL_VERTEX_SHADER);
    fsh_ID = glCreateShader(GL_FRAGMENT_SHADER);
    compile_shader(vsh_ID, vertex_shader);
    check_shader(vsh_ID);
    compile_shader(fsh_ID, fragment_shader);
    check_shader(fsh_ID);

    /* link program */
    llog(LOG_INFO, 1, "linking program");
    program_ID = glCreateProgram();
    glAttachShader(program_ID, vsh_ID);
    glAttachShader(program_ID, fsh_ID);
    glLinkProgram(program_ID);
    check_program(program_ID);
    
    glDeleteShader(vsh_ID);
    glDeleteShader(fsh_ID);

    return program_ID;
}