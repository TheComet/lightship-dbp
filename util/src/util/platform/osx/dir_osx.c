#define _SVID_SOURCE
#include "util/dir.h"
#include "util/linked_list.h"
#include "util/string.h"
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>

/* ------------------------------------------------------------------------- */
char
get_directory_listing(struct list_t* list, const char* dir)
{
    DIR* dirp = NULL;
    struct dirent* fp;

    /* open directory */
    dirp = opendir(dir);
    if(dirp == NULL)
    {
        stderr_strings(3, "Error searching directory \"", dir, "\": ");
        perror("");
        return 0;
    }

    /* copy contents of directory into linked list */
    errno = 0;
    while((fp = readdir(dirp)) != NULL)
    {
        if(!list_push(list, cat_strings(2, dir, fp->d_name)))
            return 0;
    }

    /* catch any errors */
    if(errno != 0)
        perror("Error reading directory");

    closedir(dirp);

    return (errno == 0);
}
