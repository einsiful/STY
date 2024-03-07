#include "testlib.h"
#include "pipe.h"

// You closed the write end of the pipe in the parent process.
int main() {
    test_start("pipe.c");

    char *argv1[] = {
        "/bin/ls",
        ".",
        NULL
    };
    get_output(argv1);

    test_exit(0);
    return test_end();
}
