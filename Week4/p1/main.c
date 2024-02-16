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

    startThread(0, 0);
    startThread(1, 1);
    startThread(2, 3);
    startThread(3, 1);
    startThread(4, 2);
    startThread(5, 1);
    startThread(6, 3);
    startThread(7, 5);

    // No thread is ready, so scheduler should return -1
    test_scheduleNextThread(-1);

    // Thread 1 becomes ready, so scheduler should return that thread.
    onThreadReady(1);
    test_scheduleNextThread(1);

    // Thread 1 is preempted. This means it should be put back in the ready queue
    // As the only thread in the queue, it should be scheduled to run again right away
    onThreadPreempted(1);
    test_scheduleNextThread(1);

    // Thread 1 is now blocked on some I/O. This means no thread is ready,
    // and the scheduler should return -1
    onThreadWaiting(1);
    test_scheduleNextThread(-1);
    // [Note on starvation prevention: Any lower priority threads had the chance to run here,
    // future decisions regarding starvation prevention will not consider any execution before this

    // Now, thread 1 gets the data from the disk, so it should be scheduled again
    onThreadReady(1);
    test_scheduleNextThread(1);

    // Thread 0 and 7 get ready as well...
    onThreadReady(0);
    onThreadReady(7);

    // Alright, so let's test the startvation prevention rule...
    int k = 0;
    do {
        // Thread 1, which is still running, gets preempted.
        // Now three threads are ready T0(prio 0), T1(prio 1), and T7(prio 5)
        // The scheduler should select the highest priority thread....
    	onThreadPreempted(1);
    	test_scheduleNextThread(0);

    	// Now thread 0 is running. It should scheduled for a total of 4 times.
    	for (int i = 0; i < 3; i++) {
       		onThreadPreempted(0);
        	test_scheduleNextThread(0);
    	}
    	// Now we have scheduled thread 0 four times.
    	// This means that a lower priority thread should be scheduled next.
	// This should continue until T1 has executed 4 times as well.
        //
    	onThreadPreempted(0);
        if(k < 3) {
    		test_scheduleNextThread(1);
	} else {
    		test_scheduleNextThread(7);
	}
		

        k++;
    }
    while(k < 4);

    return test_end();
}
