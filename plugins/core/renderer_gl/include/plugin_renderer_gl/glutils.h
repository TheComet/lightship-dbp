#ifdef _DEBUG
#   define printOpenGLError() printOglError(__FILE__, __LINE__)
#else
#   define printOpenGLError()
#endif

void z_error(/* should be const */char* message);
int printOglError(char *file, int line);
