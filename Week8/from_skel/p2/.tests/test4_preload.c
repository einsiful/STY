#include "testlib.h"
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <mqueue.h>

int mq_close(mqd_t mqdes) {
    if (mqdes == 42) {
        test_here('a');
    }
    return 0;
}
