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
        nbytes = read(pipefd[0], buffer, buf_size - 1); // Leave space for null terminator
    if (nbytes <= 0) { // Check for end of data or error
        break; // Exit loop if no more data or if an error occurred
    }

    buffer[nbytes] = '\0'; // Ensure buffer is null-terminated

    // Optional: Process the buffer here if you need to filter or validate content
    // For simplicity, this example directly appends valid content (numeric sequences separated by '-')
    for (int i = 0; i < nbytes; ++i) {
        if ((buffer[i] >= '0' && buffer[i] <= '9') || buffer[i] == '-') {
            // Append valid characters directly to 'result'
            size_t current_len = strlen(result);
            char valid_char[2] = {buffer[i], '\0'}; // Temp string for appending

            char *temp_result = realloc(result, current_len + 2); // +2 for char and null terminator
            if (temp_result == NULL) {
                free(result);
                return NULL;
            }
            result = temp_result;
            strcat(result, valid_char); // Append the valid character
        }
        // Optionally, handle newline or other characters specifically here
    }
}
    return result;
    }
}