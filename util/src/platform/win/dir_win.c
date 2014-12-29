#include <windows.h>
#include <stdio.h>
#include "util/dir.h"
#include "util/linked_list.h"
#include "util/string.h"
#include "util/memory.h"
#include "util/platform/win/error.h"

void
get_directory_listing(struct list_t* list, const char* dir)
{
    HANDLE fh;
    WIN32_FIND_DATA fd;
    char* search_str;

    /* windows needs a wildcard to get a complete list of files in a directory */
    search_str = (char*)MALLOC((strlen(dir)+2) * sizeof(char*));
    sprintf(search_str, "%s*", dir);

    /* open directory with search string */
    fh = FindFirstFile(search_str, &fd);
    if(fh == INVALID_HANDLE_VALUE)
    {
        char* error = get_last_error_string();
        stderr_strings(4, "Error searching directory \"", dir, "\": ", error);
        FREE(error);
        return;
    }

    /* copy directory listing into list */
    do
    {
        /* is directory */
        if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            list_push(list, cat_strings(2, dir, fd.cFileName));

    } while(FindNextFile(fh, &fd));

    FindClose(fh);
    FREE(search_str);
}
