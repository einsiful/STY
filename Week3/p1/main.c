#include "testlib.h"
#include "exploit_program.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

char *EXPLOIT_TARGET="/tmp/exploited";

// In case you are on MacOS, this is not declared in C99 mode
char    *mkdtemp(char *);


int main()
{
    test_start("run_program.c");

    // This code creates a unique random directory as a target.
    // On skel, this helps avoiding problems due to multiple students running the program with the same target
    char tdir[1024]="/tmp/exploited-XXXXXX";
    if(mkdtemp(tdir)==NULL) {
        perror("mkdtemp failed");
        goto theend;
    }
    strcat(tdir,"/exploited");
    EXPLOIT_TARGET = tdir;
    printf("Using exploit target: %s\n", EXPLOIT_TARGET);


    // Call the function to be implemented
    int res = exploit_program("./vulnerable", "hello");
    test_equals_int(res, 0);


    // Check if something is available for reading in the test pipe
    int fd = open(EXPLOIT_TARGET, O_RDONLY);
    if(fd<0) {
        perror(EXPLOIT_TARGET);
        test_failed_message("Attack target file does not exist / was not created by the attack");
    } else {
        char buf[1024];
        int cnt = read(fd, buf, 1024);
        if(cnt<=0) {
	    test_failed_message("No data in the target file");
        } else { 
	    buf[cnt]=0;
	    printf("Debug: Data written into the attack target: %s", buf);
        }
    }

    // Cleanup: remove attack target file and random directory
    close(fd);
    unlink(EXPLOIT_TARGET);
    tdir[strlen("/tmp/exploited-XXXXXX")] = 0;
    rmdir(tdir);

theend:
    return test_end();
}

