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


ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio) {
    (void) mqdes;
    (void) msg_prio;
    if (msg_len == sizeof(Message)) {
        test_here('a');
    }
    Message *cmd = (Message*) msg_ptr;
    cmd->command = CmdExit;
    return sizeof(Message);
}
