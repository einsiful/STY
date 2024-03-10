#include "testlib.h"
#include "pipe.h"
#include <unistd.h>
#include <string.h>

// You successfully retrieve the output
int main() {
    test_start("pipe.c");

    alarm(1);
    char *argv1[] = {
        "/usr/bin/echo",
        "hello world",
        NULL
    };
    char *output = get_output(argv1);
    if(strcmp(output,"hello world"))
        test_exit(0);
    else
        test_exit(1);
    return test_end();
}
