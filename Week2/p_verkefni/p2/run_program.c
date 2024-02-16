#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ERROR_CODE 255

int run_program(char *file_path, char *args[]) {
    // Check if file_path is NULL and return ERROR_CODE if it is
    if (file_path == NULL) {
        return ERROR_CODE;
    }

    // Fork a new process
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        // If args is NULL, run the program without arguments
        if (args == NULL) {
            char *default_args[] = {file_path, NULL};
            execvp(file_path, default_args);
        } else {
            // Prepend the file_path to args
            int i;
            for (i = 0; args[i] != NULL; i++); // Find the length of args

            // Allocate memory for new_args
            char **new_args = malloc((i + 2) * sizeof(char *));
            if (new_args == NULL) {
                perror("malloc");
                exit(ERROR_CODE);
            }

            // Set up new_args with file_path and args
            new_args[0] = file_path;
            for (int j = 0; j <= i; j++) {
                new_args[j + 1] = args[j];
            }

            // Execute the program with new_args
            execvp(file_path, new_args);
            free(new_args);
        }

        // If execvp() fails, exit with an error code
        perror("execvp");
        exit(ERROR_CODE);
    } else if (pid > 0) {
        // Parent process
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            // Error while waiting for the child process
            perror("waitpid");
            return ERROR_CODE;
        }

        if (WIFEXITED(status)) {
            // Child process exited normally, return its exit status
            return WEXITSTATUS(status);
        } else {
            // Child process didn't exit normally
            return ERROR_CODE;
        }
    } else {
        // Fork failed
        perror("fork");
        return ERROR_CODE;
    }

    // This should not be reached, but included to suppress warnings
    return ERROR_CODE;
}
