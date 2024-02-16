#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <inttypes.h>

/*
 * Maximum number of threads
 */
#define MAX_THREADS 10

typedef int32_t tid_t;


void initScheduler();

void onThreadReady(tid_t threadId);

void onThreadPreempted(tid_t threadId);

void onThreadWaiting(tid_t threadId);

tid_t scheduleNextThread();

int startThread(tid_t threadId);
#endif
