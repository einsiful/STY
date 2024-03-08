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
        int status;
        close(pipefd[1]);
        
    buffer[1025] = 0;
    int nbytes = read(pipefd[0], buffer, buf_size);
    if (nbytes == -1) {
        return NULL;
    }
    char *output = strchr(buffer, '\n');
    if (output != NULL) {
        *output = 0;
    }
    char *result = malloc(strlen(buffer) + 20);
    strcpy(result, buffer);

    while(1) {
        nbytes = read(pipefd[0], buffer, buf_size);
        if (nbytes <= 0) {
            break;
        }

    }
        waitpid(child_pid, &status, 0);
        close(pipefd[1]);
        close(pipefd[0]);
    return result;
    }
}
