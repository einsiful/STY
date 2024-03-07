#include "testlib.h"
#define _GNU_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int pipe(int pipefd[2]) {
    pipefd[0] = 5;
    pipefd[1] = 6;
    test_exit(1);
    return 0;
}

int pipe2(int pipefd[2], int flags) {
    pipefd[0] = 5;
    pipefd[1] = 6;
    (void) flags;
    test_exit(1);
    return 0;
}

pid_t fork(void) {
    test_exit(0);
    return 0;
}
