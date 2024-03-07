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
    //Complete the function get output to capture the output of the program started by exec. Your function should fulfill the following requirements:
// • Returns NULL on any error;
// • Return a copy of the first line of the output of the program, in a memory region allocated on the heap with malloc. A line is a string up to the first newline (‘\n’) character.
// • You may truncate the first line if it is longer than 1024 bytes.
// • You should assume that the caller of get output later calls free for this memory.
// • Close unnecessary pipe endings in the parent and child respectively as soon as possible and fully closes the pipe before returning to the caller.
// • Note: You will need to use the dup2 system call to replace the standard output file descriptor (STDOUT FILENO) with one end of your pipe before executing execvp.
// • One (secret) test will be running a program that produces a lot of output (like run- ning “cat trace-stydemo.txt”).
    // Create child process

    int pipefd[2];
    if (pipe(pipefd) == -1) {
            perror("pipe failed");
            return NULL;
    }
    pid_t child_pid = fork();
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
