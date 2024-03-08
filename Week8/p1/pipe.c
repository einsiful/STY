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

    char buffer[1025];
    int nbytes = read(pipefd[0], buffer, buf_size);
    if (nbytes == -1) {
        return NULL;
    }
    buffer[1024] = 0;
    char *output = strchr(buffer, '\n');
    if (output != NULL) {
        *output = 0;
    }
    char *result = malloc(strlen(buffer) + 20);
    strcpy(result, buffer);

    while(1){
        nbytes = read(pipefd[0], buffer, buf_size);
        if (nbytes == -1) {
            return NULL;
        }
        if (nbytes == 0) {
            break;
        }
        buffer[nbytes] = 0;
        output = strchr(buffer, '\n');
        if (output != NULL) {
            *output = 0;
        }
        result = realloc(result, strlen(result) + strlen(buffer) + 1);
        strcat(result, buffer);
    }
    return result;
    }
}