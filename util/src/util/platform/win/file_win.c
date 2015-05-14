#include "util/file.h"
#include "framework/log.h"
#include "util/memory.h"
#include <windows.h>

/* ------------------------------------------------------------------------- */
uint32_t
file_load_into_memory(const char* file_name, void** buffer, file_opts_e opts)
{
    HANDLE hFile;
    DWORD buffer_size;
    DWORD bytes_read;

    /* open file */
    hFile = CreateFile(TEXT(file_name), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "CreateFile() failed for file \"%s\"\n", file_name);
        return 0;
    }

    /* get file size */
    if(!GetFileSize(hFile, &buffer_size))
    {
        fprintf(stderr, "GetFileSizeEx() failed for file \"%s\"\n", file_name);
        return 0;
    }

    /* allocate buffer to copy file into */
    if(opts & FILE_BINARY)
        *buffer = MALLOC(buffer_size);
    else
        *buffer = MALLOC(buffer_size + sizeof(char));
    if(*buffer == NULL)
	{
        fprintf(stderr, "malloc() failed in function file_load_into_memory()\n");
		return 0;
	}
    
    /* copy file into buffer */
    ReadFile(hFile, *buffer, buffer_size, &bytes_read, NULL);
    if(buffer_size != bytes_read)
    {
        fprintf(stderr, "ReadFile() failed for file \"%s\"\n", file_name);
        return 0;
    }
    CloseHandle(hFile);

    /* append null terminator if not in binary mode */
    if((opts & FILE_BINARY) == 0)
        ((char*)(*buffer))[buffer_size] = '\0';
    
    return (uint32_t)buffer_size;
}

/* ------------------------------------------------------------------------- */
void
free_file(void* ptr)
{
    FREE(ptr);
}
