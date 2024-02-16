#include "testlib.h"
#include "run_program.h"
#include <stdlib.h>
#include <unistd.h>

int main()
{
    test_start("run_program.c");

    char *args1[] = {
        "/",
        NULL
    };
    int result1 = run_program("/bin/ls", args1);
    test_equals_int(result1, 0);

    char * args2[] = {
        NULL
    };
    int result2 = run_program("/bin/false", args2);
    test_equals_int(result2, 1);

    return test_end();
}
