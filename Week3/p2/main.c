#include "testlib.h"
#include "scheduler.h"
#include <stdio.h>

#define test_scheduleNextThread(should) ({ \
    int scheduled = scheduleNextThread(); \
    printf("%s line %d: Next Thread: %d\n", __FILE__, __LINE__, scheduled); \
    test_equals_int(scheduled, should); \
})

int main()
{
    test_start("scheduler.c");
    initScheduler();

    startThread(0);
    startThread(1);
    startThread(2);
    startThread(3);
    startThread(4);
    startThread(5);
    startThread(6);
    startThread(7);

    // Expect -1 (no thread is ready yet)
    test_scheduleNextThread(-1);

    // Let's assume we idle some time.
    onThreadReady(1);
    test_scheduleNextThread(1);

    // Let's assume Thread 1 is running here.
    onThreadPreempted(1);
    test_scheduleNextThread(1);

    // Let's assume Thread 1 is still running here.
    // Thread 1 decides to do some Disk IO.
    onThreadWaiting(1);
    test_scheduleNextThread(-1);

    // Assume we idle some time.
    // Suddenly, thread 1 gets the data from the disk.
    onThreadReady(1);
    test_scheduleNextThread(1);

    // Now thread 1 is running.
    // Thread 0 and 7 get ready.
    onThreadReady(0);
    onThreadReady(7);

    // Thread 1 is still running and gets preempted.
    onThreadPreempted(1);
    test_scheduleNextThread(0);
    onThreadPreempted(0);
    test_scheduleNextThread(7);
    onThreadPreempted(7);
    test_scheduleNextThread(1);

    return test_end();
}
