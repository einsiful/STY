#ifndef SCHEDULER_H
#define SCHEDULER_H

/*
 * The maximum priority.
 * Note: 0 is highest priorty, MAX_PRIORITY is the lowest priority
 */
#define MAX_PRIORITY 10 
/*
 * Maximum number of threads
 */
#define MAX_THREADS 20

void initScheduler();

void onThreadReady(int threadId);

void onThreadPreempted(int threadId);

void onThreadWaiting(int threadId);

int scheduleNextThread();

int startThread(int threadId, int priority);
#endif
