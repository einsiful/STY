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

    char *ptr = malloc(buf_size);
    char buffer[buf_size];
    char *output = NULL;

    if (ptr == NULL) {
        perror("malloc failed");
        return NULL;
    }

    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        return NULL;
    }

   pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("fork failed");
        close(pipefd[0]);
        close(pipefd[1]);
        return NULL;
    } else if (child_pid == 0) {
        // Child process
        close(pipefd[0]); // Close unused read end
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2 failed");
            exit(1); // Use a different exit code to indicate specific failure
        }
        close(pipefd[1]); // Close original write end after duplicating

        execvp(argv[0], argv);
        // If execvp returns, it must have failed
        perror("execvp failed");
        exit(1);
    } else {
        // Parent process
        close(pipefd[1]); // Close unused write end

        ssize_t bytes_read = read(pipefd[0], buffer, buf_size - 1);
        close(pipefd[0]); // Close read end after reading

        if (bytes_read == -1) {
            perror("read failed");
            return NULL;
        }

        buffer[bytes_read] = '\0'; // Null-terminate the string read from the child process

        // Allocate memory for output to return
        output = malloc(bytes_read + 1); // +1 for null terminator
        if (output == NULL) {
            perror("malloc failed");
            return NULL;
        }

        strncpy(output, buffer, bytes_read + 1);

        // Wait for child process to terminate and check its exit status
        int status;
        waitpid(child_pid, &status, 0);
        if (status != 0) {
            printf("Child process terminated with error. Status: %d\n", status);
            free(output); // Clean up allocated memory on error
            return NULL;
        }
    }

    return output;
}
