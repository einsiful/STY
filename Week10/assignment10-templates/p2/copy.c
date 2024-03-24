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

typedef struct {
    char *from;     // Source file path
    char *to;       // Destination file path
    int blocksize;  // Block size for copying
} CopyArgs;


int doCopy(CopyArgs* args) {
    if (args == NULL || args->from == NULL || args->to == NULL || args->blocksize <= 0) {
        return -1;
    }

    int in_fd = open(args->from, O_RDONLY);
    if (in_fd < 0) {
        perror("Error opening source file");
        return -2;
    }

    struct stat stat_buf;
    if (fstat(in_fd, &stat_buf) != 0) {
        perror("Failed to get source file permissions");
        close(in_fd);
        return -3;
    }

    int out_fd = open(args->to, O_WRONLY | O_CREAT | O_EXCL, stat_buf.st_mode & 0777);
    if (out_fd < 0) {
        perror("Error creating destination file");
        close(in_fd);
        return -4;
    }

    char *buffer = malloc(args->blocksize);
    if (buffer == NULL) {
        perror("Failed to allocate buffer");
        close(in_fd);
        close(out_fd);
        return -5;
    }

    int rd_count;
    while ((rd_count = read(in_fd, buffer, args->blocksize)) > 0) {
        int wt_count = write(out_fd, buffer, rd_count);
        if (wt_count < rd_count) { // Full write not successful
            perror("Failed to write to destination file");
            free(buffer);
            close(in_fd);
            close(out_fd);
            return -6;
        }
    }

    if (rd_count < 0) {
        perror("Error reading from source file");
        free(buffer);
        close(in_fd);
        close(out_fd);
        return -7;
    }

    free(buffer);
    close(in_fd);
    close(out_fd);
    return 0; // Success
}