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
    char *ptr = malloc(buf_size);

    if (ptr == NULL) {
        perror("malloc failed");
        return NULL;
    }

    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        return NULL;
    }

    int child_pid = fork();
    if (child_pid == -1) {
        perror("fork failed");
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
        waitpid(child_pid, &status, 0);

        ssize_t bytes_read = read(pipefd[0], buffer, buf_size);

        close(pipefd[0]);
        close(pipefd[1]);

        if (bytes_read <= 0){
            return NULL;
        }

        if (bytes_read == -1) {
            perror("read failed");
            return NULL;
        }

        else{
            int i;
            for (i = 0; i < bytes_read; i++) {
                if (buffer[i] == '\n') {
                    break;
                }
                ptr[i] = buffer[i];
            }
            ptr[i] = '\0';
        }
    }

    return ptr;
    }

