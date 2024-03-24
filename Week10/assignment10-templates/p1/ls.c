/* 
 * Group number (on canvas): xx
 * Student 1 name: xx 
 * Student 2 name: xx 
 */

#include "ls.h"

// You may not need all these headers ...
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
// #include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 1024 // Define it manually if not defined
#endif


int list(const char* path, int recursive)
{
	DIR* dir = opendir(path);
    if (!dir) {
        perror("Failed to open directory");
        return -1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skipping hidden files and directories ('.' and '..')
        if (entry->d_name[0] == '.' || strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char fullPath[PATH_MAX];
        snprintf(fullPath, PATH_MAX, "%s/%s", path, entry->d_name);

        struct stat fileInfo;
        if (lstat(fullPath, &fileInfo) != 0) {
            perror("Failed to get file information");
            closedir(dir);
            return -1;
        }

        char typeStr[PATH_MAX + 5]; // Plus extra space for type indicators and potential symlink target
        typeStr[0] = '\0'; // Initialize as empty string to concatenate later

        if (S_ISDIR(fileInfo.st_mode)) strcpy(typeStr, "/");
        else if (S_ISFIFO(fileInfo.st_mode)) strcpy(typeStr, "|");
        else if (S_ISLNK(fileInfo.st_mode)) {
            char linkTarget[PATH_MAX];
            if (readlink(fullPath, linkTarget, sizeof(linkTarget)) < 0) {
                perror("Failed to read link target");
                closedir(dir);
                return -1;
            }
            snprintf(typeStr, sizeof(typeStr), " -> %s", linkTarget);
        }
        else if (fileInfo.st_mode & S_IXUSR) strcpy(typeStr, "*"); // Executable

        char sizeStr[20];
        sprintf(sizeStr, "%ld", fileInfo.st_size);

        printLine(sizeStr, fullPath, *typeStr ? typeStr : " ");

        // Recursive call for directories
        if (recursive && S_ISDIR(fileInfo.st_mode)) {
            // Append a trailing slash to directory path if missing
            if (fullPath[strlen(fullPath) - 1] != '/') {
                strcat(fullPath, "/");
            }
            list(fullPath, recursive); // Recursive call
        }
    }

    closedir(dir);
    return 0;
}