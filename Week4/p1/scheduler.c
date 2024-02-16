#include "scheduler.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

typedef struct _QueueItem {
    /*
     * The tid of this item.
     */
    int tid;
    /*
     * The next item in the queue.
     * NULL if there is no next item.
     */
    struct _QueueItem *next;
} QueueItem;

typedef struct _Queue {
    /*
     * The first item of the queue.
     * NULL if the queue is empty.
     */
    QueueItem *head;
    /*
     * The last item of the queue.
     * undefined if the queue is empty.
     */
    QueueItem *tail;
} Queue;

typedef enum _ThreadState {
    STATE_UNUSED = 0, // This entry in the _threads array is unused.

    STATE_READY,      // The thread is ready and should be on a ready queue for
                      //   selection by the scheduler
    STATE_RUNNING,    // The thread is running and should not be on a ready queue
    STATE_WAITING     // The thread is blocked and should not be on a ready queue
} ThreadState;

typedef struct _Thread {
    int threadId;
    ThreadState state;
    /*
     * Range: 0 ... MAX_PRIORITY (including)
     * 0 is highest, MAX_PRIORITY is lowest priority
     */
    int priority;
} Thread;

Thread _threads[MAX_THREADS] = {{0}};

/* TODO: Add global variables if needed. */

/*
 * Adds a new, waiting thread.
 */
int startThread(int threadId, int priority)
{
    if (((threadId < 0) || (threadId >= MAX_THREADS) ||
        (_threads[threadId].state != STATE_UNUSED)) ||
        (priority < 0) || (priority > MAX_PRIORITY)) {

        return -1;
    }

    _threads[threadId].threadId = threadId;
    _threads[threadId].state    = STATE_WAITING;
    _threads[threadId].priority = priority;
    return 0;
}

/*
 * Append to the tail of the queue.
 * Does nothing on error.
 */
void _enqueue(Queue *queue, int tid)
{
    (void)queue;
    (void)tid;

    // TODO: Implement
}

/*
 * Remove and get the head of the queue.
 * Return -1 if the queue is empty.
 */
int _dequeue(Queue *queue)
{
    (void)queue;

    // TODO: Implement
    return -1;
}

void initScheduler()
{
    // TODO: Implement if you need to initialize any global variables you added
}

/*
 * Called whenever a waiting thread gets ready to run.
 */
void onThreadReady(int threadId)
{
    (void)threadId;

    // TODO: Implement
}

/*
 * Called whenever a running thread is forced of the CPU
 * (e.g., through a timer interrupt).
 */
void onThreadPreempted(int threadId)
{
    (void)threadId;

    // TODO: Implement
}

/*
 * Called whenever a running thread needs to wait.
 */
void onThreadWaiting(int threadId)
{
    (void)threadId;

    // TODO: Implement
}

/*
 * Gets the id of the next thread to run and sets its state to running.
 */
int scheduleNextThread()
{
    // TODO: Implement
    return -1;
}


#if 0
int main() {
	// Initially empty queue (head and tail is NULL)
	Queue q = {NULL,NULL};

	_enqueue( &q, 42 );
	_enqueue( &q, 99 );
	int x = _dequeue( &q );
	printf("Expect: 42, and I got: %d\n", x);

	x = _dequeue( &q );
	printf("Expect: 99, and I got: %d\n", x);
}
#endif
