#ifndef TESTLIB_H
#define TESTLIB_H

#include <inttypes.h>
#include <stddef.h>

void test_start(char *filename);

#define test_equals_int(is, should) _test_equals_int(is, should, #is, __FILE__, __LINE__);
void _test_equals_int(int is, int should, char *is_str, char* file, int line);

#define test_equals_int64(is, should) _test_equals_int64(is, should, #is, __FILE__, __LINE__);
void _test_equals_int64(int64_t is, int64_t should, char *is_str, char* file, int line);

#define test_equals_ptr(is, should) _test_equals_ptr(is, should, #is, __FILE__, __LINE__);
void _test_equals_ptr(void *is, void *should, char *is_str, char* file, int line);

#define test_equals_string(is, should) _test_equals_string(is, should, #is, __FILE__, __LINE__);
void _test_equals_string(char* is, char* should, char *is_str, char* file, int line);

#define test_failed_message(message) _test_failed_message(message, __FILE__, __LINE__);
void _test_failed_message(char *message, char *file, int line);

int test_end();

void test_here(unsigned char label);

void test_expect(unsigned char label);

void test_expected(unsigned char label);

void test_exit(int success);

void *get_next_function(char* name);

/**
* Start tracking mallocs.
* @param allowedMallocs How manny allocs will succeed. Use -1 if all will.
*/
void memtest_start_tracking(long allowedMallocs);
/**
* Stop using malloc override and use old malloc again.
*/
void memtest_end_tracking();
/**
* Stop using malloc. If a free is missing, exit the program.
*/
void memtest_end_tracking_require_free();
/**
* Check if a free was missing
*/
int memtest_missing_free();
/**
* Count how many frees for mallocs since the last start are missing.
*/
int memtest_missing_frees();
/**
* Check if a pointer was created with a not-freed malloc call.
*/
int memtest_is_malloced(void *ptr);
/**
* Get the size of the allocated block. Undefined if the block is not allocated.
*/
size_t memtest_get_size(void *ptr);

#define memtest_end_tracking_require_free() ({if (memtest_missing_free()) test_failed_message("Missing free."); memtest_end_tracking();})


#endif
