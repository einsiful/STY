#include <inttypes.h>

typedef struct _statistics {
	uint64_t min;
	uint64_t avg;
	uint64_t max;
} Statistics;

typedef struct _MeasureParameters {
	uint64_t memsize;
	uint64_t count;
	uint64_t step;
} MeasurementParameters;


int64_t measureFunction( int(*function)(void *), void *arg );
int accessMemory(uint64_t memsize, uint64_t count, uint64_t step);
int accessMemoryWrapper(void *p);
void executeMeasurement();


