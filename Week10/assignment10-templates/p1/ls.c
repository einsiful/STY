/* 
 * Group number (on canvas): 2
 * Student 1 name: Einar Árni Bjarnason
 * Student 2 name: Hlynur Ísak Vilmundarson
 */

#define _GNU_SOURCE
#include <dirent.h>
#include <limits.h> // We need PATH_MAX

#include "ls.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


int list(const char* path, int recursive)
{
    (void) recursive;
    DIR* dir = opendir(path);

    if (!dir) {
        return -1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;

        char fullPath[PATH_MAX];
        snprintf(fullPath, PATH_MAX, "%s/%s", path, entry->d_name);

        struct stat fileInfo;
		
        if (lstat(fullPath, &fileInfo) != 0) {
            closedir(dir);
            return -1;
        }

        char typeStr[PATH_MAX] = "";

        if (S_ISDIR(fileInfo.st_mode)) strcpy(typeStr, "/");

        else if (S_ISFIFO(fileInfo.st_mode)) strcpy(typeStr, "|");

        else if (S_ISLNK(fileInfo.st_mode)) {

            ssize_t len = readlink(fullPath, typeStr + 4, sizeof(typeStr) - 4 - 1);
            if (len != -1) {
                memcpy(typeStr, " -> ", 4);
                typeStr[len + 4] = '\0';
            } 
			
			else {
                closedir(dir);
                return -1;
            }

        } 
		else if (fileInfo.st_mode & S_IXUSR) strcpy(typeStr, "*");

        _printLine(fileInfo.st_size, fullPath, typeStr);
    }

    closedir(dir);
    return 0;
}