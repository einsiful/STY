// Authors: einararn22, hlynurv22
// Group: 2
#include "scheduler.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

typedef struct _QueueItem {
    /*
     * The data of this item (thread id (thread id)
     */
    tid_t tid;

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
    tid_t threadId;
    ThreadState state;
} Thread;

Thread _threads[MAX_THREADS] = {{0}};

/*
 * The ready queue
 */
Queue queue = {0};

/*
 * Adds a new, waiting thread.
 * The new thread is in state WAITING and not yet inserted in a ready queue.
 */
int startThread(tid_t threadId)
{
    if (((threadId < 0) || (threadId >= MAX_THREADS) ||
        (_threads[threadId].state != STATE_UNUSED))) {
        return -1;
    }

    _threads[threadId].threadId = threadId;
    _threads[threadId].state    = STATE_WAITING;
    return 0;
}

/*
 * Append to the tail of the queue.
 * Does nothing on error.
 */
void _enqueue(Queue *queue, tid_t tid)
{
    QueueItem *newItem = (QueueItem *)malloc(sizeof(QueueItem));

    if (newItem == NULL) {

        return;
    }

    newItem->tid = tid;
    newItem->next = NULL;

    if (queue->head == NULL) {

        queue->head = newItem;

    } else {

        queue->tail->next = newItem;
    }

    queue->tail = newItem;
}

/*
 * Remove and get the head of the queue.
 * Return -1 if the queue is empty.
 */
tid_t _dequeue(Queue *queue)
{
    if (queue->head == NULL) {

        return -1;
    }

    tid_t firstItemTid = queue->head->tid;

    QueueItem *temp = queue->head;
    
    queue->head = queue->head->next;

    if (queue->head == NULL) {

        queue->tail = NULL;
    }

    free(temp);

    return firstItemTid;
}

void initScheduler()
{
    for (int i = 0; i < MAX_THREADS; i++) {

        _threads[i].state = STATE_UNUSED;
    }
}

/*
 * Called whenever a waiting thread gets ready to run.
 */
void onThreadReady(tid_t threadId)
{
    if (threadId >= 0 && threadId < MAX_THREADS) {

        _threads[threadId].state = STATE_READY;

        _enqueue(&queue, threadId);
    }
}

/*
 * Called whenever a running thread is forced of the CPU
 * (e.g., through a timer interrupt).
 */
void onThreadPreempted(tid_t threadId)
{
    if (threadId >= 0 && threadId < MAX_THREADS) {

        _threads[threadId].state = STATE_READY;

        _enqueue(&queue, threadId);
    }
}

/*
 * Called whenever a running thread needs to wait.
 */
void onThreadWaiting(tid_t threadId)
{
    if (threadId >= 0 && threadId < MAX_THREADS) {

        _threads[threadId].state = STATE_WAITING;
    }
}

/*
 * Gets the id of the next thread to run and sets its state to running.
 */
tid_t scheduleNextThread()
{
    tid_t nextThread = _dequeue(&queue);

    if (nextThread >= 0) {

        _threads[nextThread].state = STATE_RUNNING;

    }

    return nextThread;
}


#if 0
int main() {
	// Initially empty queue (head and tail is NULL)
	Queue q = {NULL,NULL};

	_enqueue( &q, 42 );
	_enqueue( &q, 99 );
	tid_t x = _dequeue( &q );
	printf("Expect: 42, and I got: %d\n", x);

	x = _dequeue( &q );
	printf("Expect: 99, and I got: %d\n", x);
}
#endif
