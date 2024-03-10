#include "testlib.h"
#include "pipe.h"

// You created the pipe.
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
