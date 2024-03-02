#include "testlib.h"
#include "measure_tlb.h"
#include <stdio.h>
#include <stdlib.h>

#define REPETITIONS 1
#define ITERATIONS 15728640
#define MSTEPS 100

int main()
{
    test_start("measure_tlb.c");

    printf(" # ;  Pages ;      min/ms ;     avg/ms ;     max/ms\n");
    executeMeasurement();
    return test_end();
}




