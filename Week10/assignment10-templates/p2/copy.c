/* 
 * Group number (on canvas): 2
 * Student 1 name: Einar Árni Bjarnason
 * Student 2 name: Hlynur Ísak Vilmundarson
 */

#define _POSIX_C_SOURCE 2
#include "copy.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

// No need to change this. Parses argc into the CopyArgs structure
int parseCopyArgs(int argc, char * const argv[], CopyArgs* args)
{
	if (args == NULL) {
		return -1;
	}

	// Initialize with default values
	args->blocksize  = 4096;

	int opt;
	while ((opt = getopt(argc, argv, "b:")) != -1) {
		switch (opt) {
			case 'b':
				args->blocksize = atoi(optarg);

				if ((errno != 0) || (args->blocksize <= 0)) {
					return -1; // Invalid blocksize argument.
				}

				break;

			default: /* '?' */
				return -1;
		}
	}

	if (optind != argc - 2) {
		return -1; // We expect two parameters after the options.
	}

	args->from = argv[optind];
	args->to   = argv[optind + 1];

	return 0;
}


int doCopy(CopyArgs* args) {
    if (args == NULL || args->from == NULL || args->to == NULL || args->blocksize <= 0) {
        return -1; // Validate args and ensure blocksize is positive
    }

    int in_fd, out_fd, rd_count, wt_count;
    // Allocate the buffer dynamically based on args->blocksize
    char *buffer = (char *)malloc(args->blocksize);
    if (buffer == NULL) {
        perror("Failed to allocate memory for buffer");
        return -1;
    }

    // Open the source file
    in_fd = open(args->from, O_RDONLY);
    if (in_fd < 0) {
        perror("Error opening source file");
        free(buffer);
        return -2;
    }

    struct stat stat_buf;
    // Get the source file's permissions
    if (fstat(in_fd, &stat_buf) != 0) {
        perror("Failed to get source file permissions");
        close(in_fd);
        free(buffer);
        return -3;
    }

    // Create the destination file with source file's permissions
    out_fd = open(args->to, O_WRONLY | O_CREAT | O_EXCL, stat_buf.st_mode & 0777);
    if (out_fd < 0) {
        perror("Error opening destination file");
        close(in_fd);
        free(buffer);
        return -4;
    }

    while ((rd_count = read(in_fd, buffer, args->blocksize)) > 0) {
        wt_count = write(out_fd, buffer, rd_count);
        if (wt_count <= 0) {
            perror("Failed to write to destination file");
            close(in_fd);
            close(out_fd);
            free(buffer);
            return -5;
        }
    }

    if (rd_count < 0) {
        perror("Error reading from source file");
        close(in_fd);
        close(out_fd);
        free(buffer);
        return -6;
    }

    close(in_fd);
    close(out_fd);
    free(buffer); // Ensure the buffer is freed at the end

    return 0; // Return 0 on success
}
