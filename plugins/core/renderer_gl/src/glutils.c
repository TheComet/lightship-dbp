#include "plugin_renderer_gl/glutils.h"
#include "util/log.h"
#include "GL/glew.h"

int z_verbose = 0;
void z_error(/* should be const */char* message)
{
    llog(LOG_ERROR, 1, message);
}

int printOglError(char *file, int line)
{

    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
        llog(LOG_ERROR, 4, "glError in file %s @ line %d: %s\n",
                 file, line, gluErrorString(glErr));
        retCode = 1;
    }
    return retCode;
}
