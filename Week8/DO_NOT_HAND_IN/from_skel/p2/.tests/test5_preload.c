#include "testlib.h"
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <mqueue.h>
#include <stdarg.h>

/*
 * The message format to be sent to the server.
 */
typedef struct {
    /*
     * One of the command constants.
     */
    int command;
    /*
     * Operand 1 (if required)
     */
    int operand1;
    /*
     * Operand 2 (if required)
     */
    int operand2;
} CalculatorCommand;

#define QUEUE_NAME "/simple_calculator"

mqd_t mq_open(const char *name, int oflag, ...) {
    if (strncmp(QUEUE_NAME, name, strlen(QUEUE_NAME)) == 0) {
        test_here('a');
    }
    // O_RDONLY is 0 :-(
    // if (oflag & O_RDONLY) {
    // 	test_here('b');
    // }
    if (oflag & O_CREAT) {
        test_here('c');
        va_list argp;
        va_start(argp, oflag);
        int i = va_arg(argp, int);
        struct mq_attr *attr = va_arg(argp, struct mq_attr *);

        if ((i & 0200) == 0200) {
            test_here('d');
        }
        if (attr->mq_msgsize == sizeof(CalculatorCommand)) {
            test_here('e');
        }
        va_end(argp);
    }

    return -1;
}

