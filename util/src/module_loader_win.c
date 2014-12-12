#include <util/config.h>

#ifdef LIGHTSHIP_PLATFORM_WINDOWS

#include <stdio.h>
#include <util/string.h>
#include <Windows.h>

/*
 * Returns the last Win32 error as a char array. The string was allocated with
 * malloc() and MUST be freed.
 */
static char* get_last_error_string(void)
{
	int size;
	char* buffer = NULL;
	LPSTR messageBuffer = NULL;

    //Get the error message, if any.
    DWORD errorMessageID = GetLastError();
    if(errorMessageID == 0)
        return malloc_string("No error message has been recorded");

    size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	buffer = (char*)malloc((size+1) * sizeof(char*));
	strncpy(buffer, messageBuffer, size);
	buffer[size] = '\0';

    /* Free the buffer. */
    LocalFree(messageBuffer);

    return buffer;
}

void* module_open(const char* filename)
{
	HINSTANCE handle = LoadLibrary(filename);
    if(handle == NULL)
    {
		char* error = get_last_error_string();
		if(error)
		{
			fprintf_strings(stderr, 2, "Error loading plugin: ", error);
			free(error);
		}
    }
    return handle;
}

void* module_sym(void* handle, const char* symbol)
{
    FARPROC ptr;

    ptr = GetProcAddress((HINSTANCE)handle, symbol);
    if(ptr == NULL)
    {
        char* error = get_last_error_string();
        if(error)
        {
            fprintf_strings(stderr, 2, "Error loading plugin: ", error);
			free(error);
        }
    }
    return (void*)ptr;
}

void module_close(void* handle)
{
    FreeLibrary((HINSTANCE)handle);
}

#endif /* #ifdef LIGHTSHIP_PLATFORM_WINDOWS */