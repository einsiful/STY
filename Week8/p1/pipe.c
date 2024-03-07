// This changes the way some includes behave.
// This should stay before any include.
#define _GNU_SOURCE

#include "pipe.h"
#include <sys/wait.h> /* For waitpid */
#include <unistd.h> /* For fork, pipe */
#include <stdlib.h> /* For exit */
#include <stdio.h>
#include <string.h>
          

char *get_output(char *argv[])
{
    // Create child process
    int child_pid = fork();
    if (child_pid == -1) {
	perror("fork failed");
	return NULL;
    } else if (child_pid == 0) {
        // Replace program
        execvp(argv[0], argv);
	perror("execvp failed");
	// The child process must terminate if execp failed!
        exit(255);
    } else {
	// let's wait for the child to terminate
	int status;
        waitpid(child_pid, &status, 0);
	
	char *ptr = strdup("Fake output. Functionality not implemented");
	return ptr;
    }
}
