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
 * The commands supported by the server
 */
typedef enum _Command {
    CmdAdd = 0x00,           // Adds the two message parameters
    CmdMul,             // Multiplies the two message parameters
    CmdExit                  // Stops the server
} Command;

/*
 * The message format to be sent to the server.
 */
typedef struct _Message {
    /*
     * One of the command constants.
     */
    Command command;
    /*
     * Used as operand 1 (if required)
     */
    int parameter1;
    /*
     * Used as operand 2 (if required)
     */
    int parameter2;
} Message;


#define QUEUE_NAME "/simple_calculator"

ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio) {
    (void) mqdes;
    (void) msg_len;
    (void) msg_prio;
    Message *cmd = (Message*) msg_ptr;
    cmd->command = CmdExit;
    return sizeof(Message);
}

int mq_close(mqd_t mqdes) {
    (void) mqdes;
    test_here('a');
    return 0;
}

char qname[1024];
///mqd_t mq_open(const char *name, int oflag, mode_t mode, struct mq_attr *attr){
mqd_t mq_open(const char *name, int oflag, ...) {
    va_list args;
    va_start(args, oflag);

    strncpy(qname, name, 1024);
    mode_t mode = va_arg(args, mode_t);
    struct mq_attr *attr = va_arg(args, struct mq_attr *);
    mqd_t (*orig)(const char *name, int oflag, mode_t mode, struct mq_attr *attr);
    orig = (mqd_t (*)(const char *name, int oflag, mode_t mode, struct mq_attr *attr))get_next_function("mq_open");
    return (*orig)(name, oflag, mode, attr);
}



int mq_unlink(const char *name) {
    if (strncmp(qname, name, strlen(name)) == 0) {
        test_here('b');
    }
    int (*orig)(const char *) = (int(*)(const char *))get_next_function("mq_unlink");
    (*orig)(name);
    return 0;
}
