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
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        if(execvp(argv[0], argv) == -1)
        {
	        perror("execvp failed");
            exit(255);
            return NULL;
        }
    }
    else
    {

	    int status;
        waitpid(child_pid, &status, 0);
        
        ssize_t bytes_read = read(pipefd[0], buffer, buf_size - 1);
        
        close(pipefd[0]);
        close(pipefd[1]);
        
        if (bytes_read == -1) 
        {
            perror("read failed");
            return NULL;
        }



        else 
        {
            int i;
            for (i = 0; i < buf_size && buffer[i] != '\n'; i++) 
            {
                ptr[i] = buffer[i];
            }
            ptr[i] = '\0'; // Null-terminate the string
        }
    }
    return ptr;
}