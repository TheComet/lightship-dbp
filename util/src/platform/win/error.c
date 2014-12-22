#include <windows.h>
#include "util/platform/win/error.h"
#include "util/string.h"
#include "util/memory.h"

char* get_last_error_string(void)
{
    int size;
    char* buffer = NULL;
    LPSTR messageBuffer = NULL;

    /* Get the error message, if any. */
    DWORD errorMessageID = GetLastError();
    if(errorMessageID == 0)
        return MALLOC_string("No error message has been recorded");

    size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    buffer = (char*)MALLOC((size+1) * sizeof(char*));
    strncpy(buffer, messageBuffer, size);
    buffer[size] = '\0';

    /* Free the buffer. */
    LocalFree(messageBuffer);

    return buffer;
}
