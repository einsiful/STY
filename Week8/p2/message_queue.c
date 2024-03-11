#include "message_queue.h"
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <stdlib.h>
#include <stdio.h>

/*
 * The commands supported by the server
 */
typedef enum _Command {
    CmdAdd = 0x00,     // Adds the two message parameters
    CmdMul,            // Multiples the two message parameters
    CmdExit            // Stops the server
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

#define FORMAT_STRING_ADD      "Calc: %d + %d = %d\n"
#define FORMAT_STRING_MUL      "Calc: %d * %d = %d\n"

mqd_t startClient(void)
{
    // TODO: Open the message queue previously created by the server
    return mq_open(QUEUE_NAME, O_WRONLY);
}

int sendExitTask(mqd_t client)
{

    // TODO: Send the exit command to the server.
    Message msg;
    msg.command = CmdExit;
    return mq_send(client, (char*)&msg, sizeof(msg), 0);
}

int sendAddTask(mqd_t client, int operand1, int operand2)
{

    // TODO: Send the add command with the operands
    Message msg;
    msg.command = CmdExit;
    msg.parameter1 = operand1;
    msg.parameter2 = operand2;
    return mq_send(client, (char*)&msg, sizeof(msg), 0);
}

int sendMulTask(mqd_t client, int operand1, int operand2)
{

    // TODO: Send the mul command with the operands
    Message msg;
    msg.command = CmdExit;
    msg.parameter1 = operand1;
    msg.parameter2 = operand2;
    return mq_send(client, (char*)&msg, sizeof(msg), 0);
}

int stopClient(mqd_t client)
{
    (void)client;

    // TODO: Clean up anything on the client-side
    if ((mq_close(client) == -1) || (mq_unlink(QUEUE_NAME) == -1)){
        return -1;
    };
    return 0;

}

int runServer(void)
{
    int didExit = 0, hadError = 0;
    Message msg;

    struct mq_attr attr;
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = 10;           // Maximum number of messages in the queue
    attr.mq_msgsize = sizeof(msg);  // Maximum message size
    attr.mq_curmsgs = 0;
    (void) attr;

    // TODO:
    // Create and open the message queue. Server only needs to read from it.
    // Clients only need to write to it, allow for all users.
    mqd_t server = mq_open(QUEUE_NAME, O_RDONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, &attr);
    if(server == -1) {
	    return -1;
    };


    // This is the implementation of the server part, already completed:
    // TODO: You may have to make minor extensions in order to satisfy all requirements
    do {
        // Attempt to receive a message from the queue.
        ssize_t received = mq_receive(server, (char*)&msg, sizeof(msg), NULL);

        if (received != sizeof(msg)) {
            // This implicitly also checks for error (i.e., -1)
            hadError = 1;
            continue;
        }

        switch (msg.command)
        {
            case CmdExit:
                // End this loop.
                didExit = 1;
                break;

            case CmdAdd:
                // Print the required output.
                printf(FORMAT_STRING_ADD,
                       msg.parameter1,
                       msg.parameter2,
                       msg.parameter1 + msg.parameter2);
                break;

            case CmdMul:
                // Print the required output.
                printf(FORMAT_STRING_MUL,
                       msg.parameter1,
                       msg.parameter2,
                       msg.parameter1 * msg.parameter2);
                break;

            default:
                break;
        }
    } while (!didExit);


    // TODO
    // Close the message queue on exit and unlink it
    if (stopClient(server) == -1){
        hadError = 1;
    };

    return hadError ? -1 : 0;
}
//{}