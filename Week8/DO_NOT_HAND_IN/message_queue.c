#include <mqueue.h>          
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
    // Open the message queue previously created by the server
    mqd_t client = mq_open(QUEUE_NAME, O_WRONLY);
    if (client == -1) {
        perror("mq_open failed to open the message queue previously created by the server");
        return -1;
    }
    return client;
}

int sendExitTask(mqd_t client)
{
    // TODO: Send the exit command to the server.
    Message msg;
    msg.command = CmdExit;

    // Send the exit command to the server
    if (mq_send(client, (const char*)&msg, sizeof(msg), 0) == -1) {
        perror("mq_send failed to send exit command to the server");
        return -1;
    }
    return 0;
}

int sendAddTask(mqd_t client, int operand1, int operand2)
{
    // TODO: Send the add command with the operands
    Message msg;
    msg.command = CmdAdd;
    msg.parameter1 = operand1;
    msg.parameter2 = operand2;

    // Send the add command with the operands
    if (mq_send(client, (const char*)&msg, sizeof(msg), 0) == -1) {
        perror("mq_send failed to send add command with the operands");
        return -1;
    }
    return 0;
}

int sendMulTask(mqd_t client, int operand1, int operand2)
{
    // TODO: Send the mul command with the operands
    Message msg;
    msg.command = CmdMul;
    msg.parameter1 = operand1;
    msg.parameter2 = operand2;

    // Send the mul command with the operands
    if (mq_send(client, (const char*)&msg, sizeof(msg), 0) == -1) {
        perror("mq_send failed to send mul command with the operands");
        return -1;
    }
    return 0;
}

int stopClient(mqd_t client)
{
    mq_close(client);
    if (mq_unlink("/client") == -1) {
        perror("mq_unlink failed to unlink the message queue");
        return -1;
    }
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
    mqd_t server = mq_open(QUEUE_NAME, O_RDONLY | O_CREAT | O_EXCL, 0644, &attr);
    if (server ==  -1) {
        perror("mq_open");
        return -1;
    }


    // Clients only need to write to it, allow for all users.
    mqd_t client = mq_open("/client", O_WRONLY | O_CREAT, 0622, &attr);
    if (client == -1) {
        perror("mq_open");
        mq_close(server);
        return -1;
    }


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

    // Close the message queue on exit
    stopClient(client);
    mq_close(server);

    // Unlink the message queue
    if (mq_unlink(QUEUE_NAME) == -1) {
        perror("mq_unlink");
        hadError = 1;
        return -1;
    }

    return hadError;

}
