#include <windows.h>
#include <stdio.h>
#include "util/dir.h"
#include "util/linked_list.h"
#include "util/string.h"
#include "util/memory.h"
#include "util/platform/win/error.h"

/* ------------------------------------------------------------------------- */
char
get_directory_listing(struct list_t* list, const char* dir)
{
	HANDLE fh;
	WIN32_FIND_DATA fd;
	char* search_str;

	/* windows needs a wildcard to get a complete list of files in a directory */
	search_str = (char*)MALLOC((strlen(dir)+2) * sizeof(char*));
	if(!search_str)
		return 0;
	sprintf(search_str, "%s*", dir);

	/* open directory with search string */
	fh = FindFirstFile(search_str, &fd);
	if(fh == INVALID_HANDLE_VALUE)
	{
		char* error = get_last_error_string();
		fprintf(stderr, "Error searching directory \"%s\": %s\n", dir, error);
		FREE(search_str);
		FREE(error);
		return 0;
	}

	/* copy directory listing into list */
	do
	{
		/* is directory */
		if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			if(!list_push(list, cat_strings(2, dir, fd.cFileName)))
			{
				FindClose(fh);
				FREE(search_str);
				return 0;
			}

	} while(FindNextFile(fh, &fd));

	FindClose(fh);
	FREE(search_str);

	return 1;
}
