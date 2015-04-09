#include "plugin_renderer_gl/config.h"
#include "plugin_renderer_gl/glutils.h"
#include "GL/glew.h"
#include <stdio.h>

int z_verbose = 0;
void z_error(/* should be const */char* message)
{
    fprintf(stderr, "%s\n", message);
}

int printOglError(char *file, int line)
{

    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
        fprintf(stderr, "glError in file %s @ line %d: %s\n ", file, line, gluErrorString(glErr));
        retCode = 1;
    }
    return retCode;
}
