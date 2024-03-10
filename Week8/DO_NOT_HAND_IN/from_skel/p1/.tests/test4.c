#include "testlib.h"
#include "pipe.h"
#include <unistd.h>
#include <string.h>

// You successfully retrieve the output -- even if it is long, possibly truncating it
int main() {
    test_start("pipe.c");

    alarm(1);
    char *argv1[] = {
        "bash",
	"-c",
	"printf \%2.5000f 42",
        NULL
    };
    char *output = get_output(argv1);
    if(strncmp(output,"42.000", 6))
        test_exit(0);
    else
        test_exit(1);
    return test_end();
}
