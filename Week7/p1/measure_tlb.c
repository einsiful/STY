#define _POSIX_C_SOURCE 199309L

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "measure_tlb.h"

#define PAGESIZE 4096

int64_t measureFunction( int(*function)(void *), void *arg ) {
	// TODO: Implement! (Part P7.1a)
	// 	For this purpose, the template contains the function measureFunction. This function
	// takes a function pointer (function) and an argument pointer (arg) as parameters. You should implement code that
	// • invokes the function referenced by function, passing arg as parameter;
	// • before and after the invocation, uses the API function clock gettime (see man
	// page) to obtain a high-precision time stamp; and finally
	// • if function returns an error code (value < 0), returns -1;
	// • otherwise, returns the elapsed time (difference between the two time stamps) in nanoseconds.
	//   int64_t measureFunction(  int(*function)(void *), void *arg );

// 	 ====== TEST 1: [0.5] Tested: measureFunction correctly measures some time =====
// Testing measure_tlb.c with test1.c
// Starting to test measure_tlb.c...
// You returned 0e6 ns for a function sleeping for 2 seconds
// /home/sty24/A7/p1/.tests/test1.c line 19: You should return at least 1 seconds, but not much more than that
// You have errors in your solution, please fix them.
// x:error
// TEST FAILED
// cflags_t: -ldl -std=c99 -W -Wall

	struct timespec start, end;
	int64_t diff;
	if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
		perror("clock_gettime");
		exit(EXIT_FAILURE);
	}
	if (function(arg) < 0) {
		return -1;
	}
	if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
		perror("clock_gettime");
		exit(EXIT_FAILURE);
	}
	diff = (end.tv_sec - start.tv_sec) * 1000000000;
	diff += (end.tv_nsec - start.tv_nsec);
	return diff;
}


	

int accessMemory(uint64_t memsize, uint64_t count, uint64_t step) {
	// TODO: Implement (Part P7.1b)
	(void)memsize;
	(void)count;
	(void)step;
	return -1;
}

int accessMemoryWrapper(void *p) {
	// TODO: Implement (Part P7.1c)
	(void)p;
	return -1;
}

// Feel free to adjust: Higher: Better accuracy; Lower: Faster
#define COUNT 10000000
void executeMeasurement() {
	MeasurementParameters params;
	// Print some header...
	printf(" Memory_size ; ");
	printf("step ; cachelines ;     tlbs ; time_duration ; ");
	printf("step ; cachelines ;     tlbs ; time_duration ; ");
	printf("\n");

	// TODO: You may add code here
	while(1) {
		// TODO: You may add code here: Set memory size for this iteration
		params.memsize = 8192; // Change this line!
		printf("%12" PRIu64 " ; ", params.memsize); // Do not change printf's in here!

		// This for loop will execute two times with step=64 and step=4096
		for(uint64_t step = 64; step <= 4096; step *= 64) {
			uint64_t t1 = 0;	// use this variable for your measurement result
			(void)t1;

			// TODO: You may add code here ........... (main part of this function!)

			// Find out number number of cache lines (loc) and number of TLB entires (pages),
			// corresponding to the memory size
			// TODO: Modify the next two lines!
			uint64_t locs = 8192 / step;
			uint64_t pag = 2;

			// Do not change printf's in here!
			printf("%4" PRIu64 " ; %10" PRIu64 " ; %8" PRIu64 " ; %13.8f ; ",
				step, locs, pag, t1*0.000000001);
		}
		printf("\n");
		// TODO: Maybe you add code here as well...

		// And maybe remove this line...
		break;
	}
}




