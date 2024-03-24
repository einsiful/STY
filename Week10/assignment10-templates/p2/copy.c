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


int doCopy(CopyArgs *args) {
    if (args == NULL || args->from == NULL || args->to == NULL) {
        return -1;
    }

    int fd_from = open(args->from, O_RDONLY);
    if (fd_from == -1) {
        perror("Error opening source file");
        return -1;
    }

    // Fetch the source file's permissions to replicate them for the destination file.
    struct stat source_stat;
    if (fstat(fd_from, &source_stat) != 0) {
        perror("Failed to get source file permissions");
        close(fd_from);
        return -1;
    }

    // Using source file's permissions masked with 0777 to clean permission bits.
    int fd_to = open(args->to, O_WRONLY | O_CREAT | O_EXCL, source_stat.st_mode & 0777);
    if (fd_to == -1) {
        perror("Error opening destination file");
        close(fd_from);
        return -1;
    }

    char *buffer = malloc(args->blocksize);
    if (buffer == NULL) {
        perror("Failed to allocate memory for buffer");
        close(fd_from);
        close(fd_to);
        return -1;
    }

    ssize_t bytes_read;
    while ((bytes_read = read(fd_from, buffer, args->blocksize)) > 0) {
        int is_zero = 1;
        for (ssize_t i = 0; i < bytes_read; ++i) {
            if (buffer[i] != '\0') {
                is_zero = 0;
                break;
            }
        }

        if (is_zero) {
            // Use lseek to create a sparse block.
            off_t result = lseek(fd_to, bytes_read, SEEK_CUR);
            if (result == (off_t)-1) {
                perror("lseek error");
                free(buffer);
                close(fd_from);
                close(fd_to);
                return -1;
            }
        } else {
            if (write(fd_to, buffer, bytes_read) != bytes_read) {
                perror("Failed to write to destination file");
                free(buffer);
                close(fd_from);
                close(fd_to);
                return -1;
            }
        }
    }

    if (bytes_read == -1) {
        perror("Error reading from source file");
        free(buffer);
        close(fd_from);
        close(fd_to);
        return -1;
    }

    close(fd_from);
    close(fd_to);
    return 0;
}
