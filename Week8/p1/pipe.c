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
    if (nbytes == -1) {
        // Handle error
        free(result); // Avoid memory leak
        return NULL;
    }
    if (nbytes == 0) {
        // No more data to read
        break;
    }
    buffer[nbytes] = '\0'; // Properly null-terminate the new segment

    // Process the buffer to replace '\n' with the desired character
    // Here we simply ensure it's correctly terminated, adjust as needed.
    for(int i = 0; i < nbytes; ++i) {
        if(buffer[i] == '\n') {
            buffer[i] = '-'; // Replace newline with '-', adjust this as needed
        }
    }

    // Resize 'result' to accommodate the new segment
    size_t current_len = strlen(result);
    char *temp_result = realloc(result, current_len + nbytes + 1); // +1 for null terminator
    if (temp_result == NULL) {
        // Handle realloc failure
        free(result); // Avoid memory leak
        return NULL;
    }
    result = temp_result;

    // Append the new segment to 'result'
    strcat(result, buffer);
}
    return result;
    }
}