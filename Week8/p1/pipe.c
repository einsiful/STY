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

    size_t current_length = 0; // Keep track of the current length of 'result'
result = malloc(1); // Allocate a minimal initial buffer
if (!result) {
    perror("Initial malloc failed");
    return NULL;
}
result[0] = '\0'; // Ensure the buffer is initially an empty string

while (1) {
    nbytes = read(pipefd[0], buffer, buf_size - 1); // Read from the pipe
    if (nbytes <= 0) { // No more data or an error
        break;
    }

    buffer[nbytes] = '\0'; // Null-terminate the buffer

    for (int i = 0; i < nbytes; i++) {
        if ((buffer[i] >= '0' && buffer[i] <= '9') || buffer[i] == '-') {
            // Ensure enough space for one more character and a null terminator
            char *temp_result = realloc(result, current_length + 2);
            if (!temp_result) {
                perror("realloc failed");
                free(result);
                return NULL;
            }
            result = temp_result;
            
            result[current_length++] = buffer[i]; // Append the character
            result[current_length] = '\0'; // Re-null-terminate
        }
    }
}

// After loop: 'result' should contain only the filtered output
return result;
    }
}