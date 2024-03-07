#include "testlib.h"
#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <stdio.h>


int fd1 = 3;
int fd2 = 4;

int pipe(int pipefd[2]) {
    pipefd[0] = fd1;
    pipefd[1] = fd2;
    return 0;
}

int pipe2(int pipefd[2], int flags) {
    pipefd[0] = fd1;
    pipefd[1] = fd2;
    (void) flags;
    return 0;
}

pid_t fork(void) {
    return 42;
}

int close(int fd) {
    if (fd == fd1) {
        printf("Read end closed.\n");
        fd1 = 0;
    }
    if (fd == fd2) {
        printf("Write end closed.\n");
        fd2 = 0;
        test_exit(1);
    }
    return 0;
}

int execvp(const char *file, char *const argv[]) {
    (void) file;
    (void) argv;
    return -1;
}


