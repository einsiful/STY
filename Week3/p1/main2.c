//make mkdtemp available on MacOS
#define _DARWIN_C_SOURCE

#include "testlib.h"
#include "exploit_program.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

char *EXPLOIT_TARGET="/tmp/exploited";

#define PROGNAME "./vulnerable"

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
    int res = exploit_program(PROGNAME, "hello");
    test_equals_int(res, 0);


    // Check if something is available for reading in the test pipe
    int fd = open(EXPLOIT_TARGET, O_RDONLY);
    if(fd<0) {
        perror(EXPLOIT_TARGET);
   printf("*** Target file does not exist: %s. Seems like your exploit failed (will not pass any of the skel tests)\n", EXPLOIT_TARGET);
        test_failed_message("Attack target file does not exist / was not created by the attack");
    } else {
        char buf[1024];
        int cnt = read(fd, buf, 1024);
        if(cnt<=0) {
       printf("*** No data in the target file %s. Seems like your exploit failed (will not pass any of the skel tests)\n", EXPLOIT_TARGET);
     test_failed_message("No data in the target file");
        } else { 
            printf("*** you should pass Test 0: Your exploit caused the vulnerable program to write something to %s\n", EXPLOIT_TARGET);
     buf[cnt]=0;
     printf("Debug: Data written into the attack target: %s", buf);
     if (strncmp(buf+strlen("xxxx-xx-xx xx:xx:xx: "), "hello", 5) ) {
  // Not the expected data...
  test_failed_message("Wrong data in the target log file (expected timestamp and message)");
     } else {
  printf("*** you should also pass Test 1: You caused writing a timestamp followed by 'hello' to %s\n", EXPLOIT_TARGET);
     }
            if (strcmp(buf+strlen("xxxx-xx-xx xx:xx:xx: "), "hello") &&
                strcmp(buf+strlen("xxxx-xx-xx xx:xx:xx: "), "hello\n") ) {
                // Not the expected data...
                test_failed_message("Wrong data in the target log file (expected timestamp and message and nothing else)");
            } else {
                printf("*** you should also pass Test 2: You caused writing a timestamp followed by 'hello' to %s, and nothing else.\n", EXPLOIT_TARGET);
            } 
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