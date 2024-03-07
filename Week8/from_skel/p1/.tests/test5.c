#include "testlib.h"
#include "pipe.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// You successfully retrieve the output
int main() {
    test_start("pipe.c");

    alarm(1);

    memtest_start_tracking(-1);     

    char *argv1[] = {
        "/usr/bin/echo",
        "hello world",
        NULL
    };
    char *output = get_output(argv1);
    if(strcmp(output,"hello world"))
        test_exit(0);
    if(!memtest_is_malloced(output)){
	printf("Memory that is returned was not allocated with malloc?\n");
	test_exit(0);
    } else {
        free(output);
        int n = memtest_missing_free();
        if(n>0) {
	    printf("There is a memory leak: missing frees: %d\n", n);
            test_exit(0);
	} else {
            test_exit(1);
	}
    }
    return test_end();
}
