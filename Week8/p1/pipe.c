#define _GNU_SOURCE
#include <sys/wait.h> // For waitpid
#include <unistd.h> // For fork, pipe, dup2
#include <stdlib.h> // For malloc, free, exit
#include <stdio.h> // For perror, printf
#include <string.h> // For memcpy

#define BUFFER_SIZE 1024

char* get_output(char* argv[]) {
    int pipefd[2];
    pid_t child_pid;
    char buffer[BUFFER_SIZE];
    char* output = NULL;
    size_t total_bytes_read = 0;

    // Initialize pipe
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        return NULL;
    }

    // Forking a child process
    child_pid = fork();
    if (child_pid == -1) {
        perror("fork failed");
        close(pipefd[0]); // Close read end
        close(pipefd[1]); // Close write end
        return NULL;
    } else if (child_pid == 0) { // Child process
        close(pipefd[0]); // Child doesn't read from pipe
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
        close(pipefd[1]); // Close the original write end of the pipe

        execvp(argv[0], argv);
        // If execvp returns, it must have failed
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }

    // Parent process
    close(pipefd[1]); // Parent doesn't write to pipe
    ssize_t bytes_read;

    // Dynamically allocate memory for output
    output = malloc(BUFFER_SIZE);
    if (!output) {
        perror("malloc failed");
        close(pipefd[0]);
        waitpid(child_pid, NULL, 0);
        return NULL;
    }

    while ((bytes_read = read(pipefd[0], buffer, BUFFER_SIZE)) > 0) {
        char* new_output = realloc(output, total_bytes_read + bytes_read + 1); // +1 for null terminator
        if (!new_output) {
            perror("realloc failed");
            free(output);
            close(pipefd[0]);
            waitpid(child_pid, NULL, 0);
            return NULL;
        }
        output = new_output;
        memcpy(output + total_bytes_read, buffer, bytes_read);
        total_bytes_read += bytes_read;
    }
    output[total_bytes_read] = '\0'; // Null-terminate the output

    if (bytes_read == -1) {
        perror("read failed");
        free(output);
        output = NULL;
    }

    close(pipefd[0]); // Close the read end of the pipe
    waitpid(child_pid, NULL, 0); // Wait for the child process to exit

    return output; // Return the captured output
}
