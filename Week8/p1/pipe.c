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
        perror("fork failed");
        close(pipefd[READ_END]);
        close(pipefd[WRITE_END]);
        return NULL;
    } else if (child_pid == 0) {
        close(pipefd[READ_END]);
        dup2(pipefd[WRITE_END], STDOUT_FILENO);
        close(pipefd[WRITE_END]);

        execvp(argv[0], argv);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else {
        close(pipefd[WRITE_END]);

        char buf[1024];
        ssize_t n;

        while(1) {
            n = read(pipefd[READ_END], buf, sizeof(buf));
            if (n == -1) {
                perror("read failed");
                close(pipefd[READ_END]);
                return NULL;
            } else if (n == 0) {
                break;
            }

            output = realloc(output, output_size + n + 1);
            if (output == NULL) {
                perror("realloc failed");
                close(pipefd[READ_END]);
                return NULL;
            }

            memcpy(output + output_size, buf, n);
            output_size += n;
            output[output_size] = '\0';

            if (strchr(buf, '\n') != NULL) {
                break;
            }
        }
    }
        return output;
    }
