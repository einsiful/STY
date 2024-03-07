#define _GNU_SOURCE

#include "pipe.h"
#include <sys/wait.h> /* For waitpid */
#include <unistd.h> /* For fork, pipe */
#include <stdlib.h> /* For exit */
#include <stdio.h>
#include <string.h>

#define READ_END  0
#define WRITE_END 1

char* get_output(char *argv[]) {
    int pipefd[2];
    pid_t child_pid;
    char *output = NULL;
    size_t output_size = 0;

    if (argv == NULL) {
        return NULL;
    }

    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        return NULL;
    }

    child_pid = fork();
    if (child_pid == -1) {
        close(pipefd[READ_END]);
        close(pipefd[WRITE_END]);
        return NULL;
    } else if (child_pid == 0) {
        close(pipefd[READ_END]);
    dup2(pipefd[WRITE_END], STDOUT_FILENO);

        execvp(argv[0], argv);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else {
        int status;

        close(pipefd[WRITE_END]);
    
    char buffer[1024];
    int n = read(pipefd[READ_END], buffer, 1024);
    if (n == 0) {
        return NULL;
    }
    buffer[1024] = 0;

    char *newline = strchr(buffer, '\n');
    if (newline != NULL) {
        *newline = 0;
    }

    char *ptr = malloc(strlen(buffer) + 20);

    strcpy(ptr, buffer);

    while((n = read(pipefd[READ_END], buffer, 1024)) > 0) {
        buffer[n] = 0;
        ptr = realloc(ptr, strlen(ptr) + n + 20);
        strcat(ptr, buffer);
    }

        waitpid(child_pid, &status, 0);
        close(pipefd[WRITE_END]);

        close(pipefd[READ_END]);

        return output;
    }
