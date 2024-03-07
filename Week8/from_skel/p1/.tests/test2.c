#include "testlib.h"
#include "pipe.h"
#include <unistd.h>

// You don't deadlock if execvp fails
int main() {
    test_start("pipe.c");

    alarm(1);
    char *argv1[] = {
        "/bin/nonexisting",
        ".",
        NULL
    };
    get_output(argv1);

    test_exit(1);
    return test_end();
}
