/* 
 * Group number (on canvas): xx
 * Student 1 name: xx
 * Student 2 name: xx
 */

#define _GNU_SOURCE
#include <dirent.h>

#include "ls.h"

// You may not need all these headers ...
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
	DIR *opened_directory;
	int error;
	opened_directory = opendir(path);
	if (opened_directory == NULL)
	{
		return -1;
	}

	struct dirent *new_file;
	struct stat new_file_statistics;
	size_t size;
	char* full_path_and_name = malloc(sizeof(char) * (MAX_FILE_NAME_LENGTH + strlen(path)));
	char type_str[MAX_FILE_NAME_LENGTH];
	char new_path[MAX_FILE_NAME_LENGTH]; // Just gonna assume the path aint that big :)

	new_file = readdir(opened_directory);
	while (new_file != NULL)
	{
		if (new_file->d_name[0] == '.')
		{
			new_file = readdir(opened_directory);
			continue;
		}

		strcpy(full_path_and_name, path);
		strcat(full_path_and_name, "/");
		strcat(full_path_and_name, new_file->d_name);

		strcpy(new_path, path);
		strcpy(new_path, "/");

		error = fstatat(dirfd(opened_directory), full_path_and_name, &new_file_statistics, AT_SYMLINK_NOFOLLOW);
		if (error != 0)
		{
			free(full_path_and_name);
			return -1;
		}
		size = new_file_statistics.st_size;
		
		if (new_file->d_type == DT_FIFO)
		{
			strcpy(type_str, "|");
		}
		else if (new_file->d_type == DT_DIR)
		{
			strcpy(type_str, "/");
		}
		else if (new_file->d_type == DT_LNK)
		{
			strcpy(type_str, " -> ");
			char* temp_str = malloc(sizeof(char) * (MAX_FILE_NAME_LENGTH));
			error = readlink(full_path_and_name, temp_str, sizeof(temp_str));
			if (error != -1)
			{
				free(full_path_and_name);
				return -1;
			}
			strcat(type_str, temp_str);
			temp_str[error] = '\0';
		}
		else if (new_file_statistics.st_mode & 0111) // If its an executable for any permission
		{
			strcpy(type_str, "*");
		}
		
		_printLine(size, full_path_and_name, type_str);
		new_file = readdir(opened_directory);
	}

	error = closedir(opened_directory);
	free(full_path_and_name);
	return error;
}