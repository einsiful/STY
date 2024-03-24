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
    if (args == NULL) {
        return -1;
    }

	#define BUF_SIZE 4096

	int fd_from = open(args->from, O_RDONLY);
	if (fd_from == -1) {
		return -1;
	}

	int fd_to = open(args->to, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd_to == -1) {
		close(fd_from);
		return -1;
	}

	char buf[BUF_SIZE];
	ssize_t bytesRead;
	while ((bytesRead = read(fd_from, buf, BUF_SIZE)) > 0) {
		ssize_t bytesWritten = write(fd_to, buf, bytesRead);
		if (bytesWritten != bytesRead) {
			close(fd_from);
			close(fd_to);
			return -1;
		}
	}

	close(fd_from);
	close(fd_to);

	return 0;



}