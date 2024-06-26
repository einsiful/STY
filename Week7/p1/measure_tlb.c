#define _POSIX_C_SOURCE 199309L

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "measure_tlb.h"

#define PAGESIZE 4096

int64_t measureFunction( int(*function)(void *), void *arg ) {
	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);
	int ret = function(arg);
	clock_gettime(CLOCK_MONOTONIC, &end);
	if (ret < 0) {
		return -1;
	}
	return (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);

	
}

int accessMemory(uint64_t memsize, uint64_t count, uint64_t step) {
	uint64_t *mem = (uint64_t*)malloc(memsize);
	if(mem == NULL) {
		return -1;
	}
	uint64_t steps = 0;
	for(uint64_t i = 0; i < count; i++) {
		steps = (i*step) % memsize;
		mem[steps/sizeof(uint64_t)] += i;
	}
	free(mem);
	return 0;
}

int accessMemoryWrapper(void *params) {
	MeasurementParameters* new_params = (MeasurementParameters*)params;
	uint64_t memsize = new_params->memsize; 
	uint64_t count = new_params->count; 
	uint64_t step = new_params->step;

	return accessMemory(memsize, count, step);
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

	params.memsize = PAGESIZE;


	while(params.memsize < 1024*1024*1024) {
	
		params.memsize = params.memsize * 2; // Change this line!
		printf("%12" PRIu64 " ; ", params.memsize); // Do not change printf's in here!

		// This for loop will execute two times with step=64 and step=4096
		for(uint64_t step = 64; step <= 4096; step *= 64) {
			uint64_t t1 = 0;	// use this variable for your measurement result
			params.step = step;
			params.count = COUNT;

			t1 = measureFunction((*accessMemoryWrapper), &params);


			// Find out number number of cache lines (loc) and number of TLB entires (pages),
			// corresponding to the memory size
			uint64_t locs = params.memsize / step;
			uint64_t pag = params.memsize / PAGESIZE;

			// Do not change printf's in here!
			printf("%4" PRIu64 " ; %10" PRIu64 " ; %8" PRIu64 " ; %13.8f ; ",
				step, locs, pag, t1*0.000000001);
		}
		printf("\n");

	}
}




