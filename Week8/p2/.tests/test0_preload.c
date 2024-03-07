#include "testlib.h"
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <mqueue.h>

#define QUEUE_NAME "/simple_calculator"

mqd_t mq_open(const char *name, int oflag, ...) {
    if (strncmp(QUEUE_NAME, name, strlen(QUEUE_NAME)) == 0) {
        test_here('a');
    }
    // accept rd/wr as well...
    if ((oflag & O_WRONLY) || (oflag & O_RDWR)) {
        test_here('b');
    }
    return 42;
}

