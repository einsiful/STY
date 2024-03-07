#include "testlib.h"
#include "pipe.h"
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

int main()
{
    test_start("pipe.c");

    printf("Running echo 'hello world'\n");
    char *argv1[] = {
        "echo",
        "hello world",
        NULL
    };
    char *r1 = get_output(argv1);
    printf("This should be 'hello world': %s\n\n", r1);
    free(r1);

    printf("Running gcc --version\n");
    char *argv2[] = {
        "gcc",
        "--version",
        NULL
    };
    char *r2 = get_output(argv2);
    printf("This should be the version string of your compiler: %s\n\n", r2);

    return test_end();
}
