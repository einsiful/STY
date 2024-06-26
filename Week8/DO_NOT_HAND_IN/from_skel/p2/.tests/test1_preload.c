#include "testlib.h"
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <mqueue.h>

/*
 * The commands supported by the server
 */
typedef enum _Command {
    CmdAdd = 0x00,           // Adds the two message parameters
    CmdMul,                  // Multiplies the two message parameters
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

int mq_send(mqd_t mqdes, const char *msg_ptr,
                     size_t msg_len, unsigned msg_prio) {
    if (mqdes == 42) {
        test_here('a');
    }
    if (msg_len == sizeof(Message)) {
        test_here('b');
    }
    Message* cmd = (Message*) msg_ptr;
    if (msg_len >= sizeof(Message) && cmd->command == CmdAdd && cmd->parameter1 == 43 && cmd->parameter2 == 44) {
        test_here('c');
    }
    (void) msg_prio;
    return 0;
}

mqd_t mq_open(const char *name, int oflag, ...) {
    (void) name;
    (void) oflag;
    return 42;
}


int mq_unlink(const char *name) {
    (void) name;
    return 0;
}

