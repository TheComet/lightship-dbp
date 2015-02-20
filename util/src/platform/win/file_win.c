#include "util/file.h"
#include "util/log.h"
#include "util/memory.h"
#include <Windows.h>

/* ------------------------------------------------------------------------- */
uint32_t
file_load_into_memory(const char* file_name, void** buffer, file_opts_e opts)
{
	HANDLE hFile;
	LARGE_INTEGER buffer_size;
	intptr_t bytes_read;

	/* open file */
	hFile = CreateFile(TEXT(file_name), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		llog(LOG_ERROR, NULL, 3, "CreateFile() failed for file \"", file_name, "\"");
		return 0;
	}

	/* get file size */
	if(!GetFileSizeEx(hFile, &buffer_size))
	{
		llog(LOG_ERROR, NULL, 3, "GetFileSizeEx() failed for file \"", file_name, "\"");
		return 0;
	}

	/* allocate buffer to copy file into */
	if(opts & FILE_BINARY)
		*buffer = MALLOC((intptr_t)buffer_size.QuadPart);
	else
		*buffer = MALLOC((intptr_t)buffer_size.QuadPart + sizeof(char));
	if(*buffer == NULL)
        OUT_OF_MEMORY("file_load_into_memory()", 0);
    
    /* copy file into buffer */
    ReadFile(hFile, *buffer, (DWORD)buffer_size.QuadPart, &bytes_read, NULL);
	if((intptr_t)buffer_size.QuadPart != bytes_read)
    {
        llog(LOG_ERROR, NULL, 3, "ReadFile() failed for file \"", file_name, "\"");
        return 0;
    }
    CloseHandle(hFile);

    /* append null terminator if not in binary mode */
    if((opts & FILE_BINARY) == 0)
        ((char*)(*buffer))[buffer_size.QuadPart] = '\0';
    
    return (uint32_t)buffer_size.QuadPart;
}

/* ------------------------------------------------------------------------- */
void
free_file(void* ptr)
{
	FREE(ptr);
}