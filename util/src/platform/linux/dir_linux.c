#define _SVID_SOURCE
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <util/dir.h>
#include <util/linked_list.h>
#include <util/string.h>

void get_directory_listing(struct list_t* list, const char* dir)
{
    DIR* dirp;
    struct dirent* dp;

    /* open directory */
    dirp = opendir(dir);
    if(!dirp)
    {
        fprintf_strings(stderr, 3, "Error searching directory \"", dir, "\": ");
        perror("");
        return;
    }

    /* copy contents of directory into linked list */
    errno = 0;
    while (!(dp = readdir(dirp)));
    {
        list_push(list, cat_strings(2, dir, dp->d_name));
    }

    /* catch any errors */
    if(errno != 0)
        perror("Error reading directory");

    closedir(dirp);
}
