#define _GNU_SOURCE
#include "testlib.h"
#include "message_queue.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// Your server gives the right output.
int main() {
    test_start("message_queue.c");
    mq_unlink("/simple_calculator");
    system("rm /dev/mqueue/*");
    int filedes[2];
    if (pipe(filedes) == -1) {
        perror("pipe");
        test_exit(0);
    }
    fcntl(filedes[1], F_SETPIPE_SZ, 4096 * 16);

    int pid = fork();
    switch(pid) {
    case -1:
        perror("fork");
        exit(0);
    case 0:
        close(filedes[0]);
        dup2(filedes[1], STDOUT_FILENO);
        close(filedes[1]);
        // Child is server
        printf("Starting test server\n");
        if (runServer() < 0) {
            perror("runServer");
            exit(1);
        }
        printf("Test server done\n");
        exit(0);
        break;
    default:
        close(filedes[1]);
        // Hope that the server is started now.
        sleep(1);
        // Send some test commands to server.
        printf("Running test client\n");
        mqd_t clientHandle = startClient();
        if (clientHandle < 0) {
            perror("startClient");
            exit(1);
        }
        if (sendAddTask(clientHandle, 10, -2) < 0) {
            perror("sendAddTask");
        }
        if (sendMulTask(clientHandle, 5, 13) < 0) {
            perror("sendMulTask");
        }
        if (sendExitTask(clientHandle) < 0) {
            perror("sendExitTask");
        }
        if (stopClient(clientHandle) < 0) {
            perror("sendExitTask");
        }
        printf("Test client done\n");
        wait(NULL);

        char out[4096 * 16];
        int count = read(filedes[0], out, sizeof(out));
        if (count < 0) {
            perror("read");
            test_exit(0);
        }
        printf("Server output:\n");
        write(STDOUT_FILENO, out, count);

        const char* expected = "10+-2=8\n5*13=65\n";
        test_contains_chars(out, expected);
	kill(pid, 15); // kill the server
    }

    return test_end();
}
