#define _GNU_SOURCE

#include "pipe.h"
#include <sys/wait.h> /* For waitpid */
#include <unistd.h> /* For fork, pipe */
#include <stdlib.h> /* For exit */
#include <stdio.h>
#include <string.h>

#define buf_size 1024

char* get_output(char *argv[]) {

    int pipefd[2];

    char buffer[buf_size];

    if (argv == NULL) {
        return NULL;
    }

    if (pipe(pipefd) == -1) {
        return NULL;
    }

    int child_pid = fork();
    if (child_pid == -1) {
        close(pipefd[0]);
        close(pipefd[1]);
        return NULL;
    }
    else if (child_pid == 0) 
    {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        execvp(argv[0], argv);
        perror("execvp failed");
        exit(255);
    }
    else {
        close(pipefd[1]);
        int status;
        waitpid(child_pid, &status, 0);
        int bytes = read(pipefd[0], buffer, buf_size);
        if (bytes == -1) {
            close(pipefd[0]);
            return NULL;
        }
        buffer[bytes] = '\0';
        close(pipefd[0]);
    }

    return strdup(buffer);
}

