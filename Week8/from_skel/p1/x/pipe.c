// This changes the way some includes behave.
// This should stay before any include.
#define _GNU_SOURCE

#include "pipe.h"
#include <sys/wait.h> /* For waitpid */
#include <unistd.h> /* For fork, pipe */
#include <stdlib.h> /* For exit */
//#include <fcntl.h>
//#include <errno.h>
#include <stdio.h>
#include <string.h>
          

#define READ_END  0
#define WRITE_END 1

char *get_output(char *argv[])
{
    int pipefd[2];

    if (argv == NULL) {
	NULL;
    }

    // We create the new pipe.
    // O_CLOEXEC is given so that the pipe is automatically closed when
    // execvp succeeds. This is necessary so we do not leak the pipe handle to
    // new exec'ed program. Setting the flag later would also work, but this
    // solution is cleaner (see man page for more information).
    if (pipe(pipefd) == -1) {
	NULL;
    }

    // We can now use pipefd[READ_END] to read from the pipe and
    // pipefd[WRITE_END] to write to it.

    int child_pid = fork();
    if (child_pid == -1) {
        close(pipefd[READ_END]);
        close(pipefd[WRITE_END]);

	return NULL;
    } else if (child_pid == 0) {
        // The child process won't need to read from the pipe.
        // It is good practice to close the unneeded end of the pipe
        // as soon as possible.
        close(pipefd[READ_END]);
	dup2(pipefd[WRITE_END], STDOUT_FILENO);

        // Replace program
        execvp(argv[0], argv);

 	// execvp failed()
	perror("execvp failed");
        exit(255);
    } else {
        int status;

        // The parent does not need to write to the pipe.
        // Close it as soon as possible.
        close(pipefd[WRITE_END]);
	printf("Parent\n");
	char buf[1025];
	int n = read(pipefd[READ_END], buf, 1024);
	printf("n=%d\n", n);
	if (n<= 0) { return NULL; }
	buf[1024]=0;
	char *nl = strchr(buf, '\n');
	if(nl) { *nl = 0; }
	char *ptr = malloc(strlen(buf)+20);
	//ptr+=15;
	strcpy(ptr, buf);
	printf("Reading\n");
	while(1)  {
		n = read(pipefd[READ_END], buf, 1024);
		if (n<=0) break;
		printf("n=%d\n", n);;
	}
	printf("Done\n");

        waitpid(child_pid, &status, 0);
        close(pipefd[WRITE_END]);

        // Clean up the leftover read end of the pipe. At this point errno is
        // set to the value we would like to present to the caller of this
        // function. Since the close call could change errno, we save and
        // restore it.
        close(pipefd[READ_END]);
	printf("Output is %s\n", ptr);
	return ptr;
    }
}
