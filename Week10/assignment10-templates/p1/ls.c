/* 
 * Group number (on canvas): xx
 * Student 1 name: xx
 * Student 2 name: xx
 */

#define _GNU_SOURCE
#include <dirent.h>
#include <limits.h> // For PATH_MAX

#include "ls.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

// Assuming _printLine is defined somewhere and has a prototype like this:
// void _printLine(size_t size, const char* full_path_and_name, const char* type_str);

int list(const char* path, int recursive)
{
    (void) recursive; // Ignored in the current implementation
    DIR* opened_directory = opendir(path);
    if (!opened_directory) {
        perror("Failed to open directory");
        return -1;
    }

    struct dirent* new_file;
    while ((new_file = readdir(opened_directory)) != NULL) {
        if (new_file->d_name[0] == '.') continue; // Skip hidden files

        char full_path_and_name[PATH_MAX];
        snprintf(full_path_and_name, PATH_MAX, "%s/%s", path, new_file->d_name);

        struct stat new_file_statistics;
        if (fstatat(dirfd(opened_directory), new_file->d_name, &new_file_statistics, AT_SYMLINK_NOFOLLOW) != 0) {
            perror("Failed to get file statistics");
            closedir(opened_directory);
            return -1;
        }

        char type_str[PATH_MAX] = ""; // Initialize to empty string
        if (new_file->d_type == DT_FIFO) {
            strcpy(type_str, "|");
        } else if (new_file->d_type == DT_DIR) {
            strcpy(type_str, "/");
        } else if (new_file->d_type == DT_LNK) {
            ssize_t link_size = readlink(full_path_and_name, type_str, sizeof(type_str) - 4); // Leave space for " -> "
            if (link_size >= 0) {
                type_str[link_size] = '\0'; // Null-terminate the result
                memmove(type_str + 4, type_str, strlen(type_str) + 1); // Shift to make space for " -> "
                memcpy(type_str, " -> ", 4); // Prepend " -> "
            } else {
                perror("Failed to read symlink");
                closedir(opened_directory);
                return -1;
            }
        } else if (new_file_statistics.st_mode & S_IXUSR) {
            strcpy(type_str, "*");
        }

        _printLine(new_file_statistics.st_size, full_path_and_name, type_str);
    }

    closedir(opened_directory);
	free(new_file);
    return 0;
}