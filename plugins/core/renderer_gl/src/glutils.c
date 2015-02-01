#include "plugin_renderer_gl/config.h"
#include "plugin_renderer_gl/glutils.h"
#include "util/log.h"
#include "GL/glew.h"

int z_verbose = 0;
void z_error(/* should be const */char* message)
{
    llog(LOG_ERROR, PLUGIN_NAME, 1, message);
}

int printOglError(char *file, int line)
{

    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
        char line_str[16];
        sprintf(line_str, "%d", line);
        llog(LOG_ERROR, PLUGIN_NAME, 7, "", "glError in file ", file, " @ line ",
             line_str, ": ", gluErrorString(glErr));
        retCode = 1;
    }
    return retCode;
}
