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
	#define OUTPUT 0200

	int in_fd, out_fd, rd_count, wt_count;
	char buffer[BUF_SIZE];

	in_fd = open(args->from, O_RDONLY);
	if (in_fd < 0) {
		exit(2);
	}

	out_fd = creat(args->to, OUTPUT);
	if (out_fd < 0) {
		exit(3);
	}

	while (1) {
		rd_count = read(in_fd, buffer, BUF_SIZE);
		if (rd_count <= 0) {
			break;
		}

		wt_count = write(out_fd, buffer, rd_count);
		if (wt_count <= 0) {
			exit(4);
		}
	}

	close(in_fd);
	close(out_fd);

	if (rd_count == 0) {
		exit(0);
	}
	else {
		exit(5);
	}

}