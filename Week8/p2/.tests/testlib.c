#define _GNU_SOURCE
#define __GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#include "testlib.h"

#define STR(x) #x
#define XSTR(x) STR(x)
#define KEY XSTR(RANDKEY)

#define DEBUG(...)
//#define DEBUG printf

typedef struct {
    void *ptr;
    int freed :1;
    size_t size;
} MemtestHistoryEntry;
#define MALLOC_HISTORY 10000

typedef struct {
    // How many mallocs will succeed.
    long succeding_calls;
    // Mallocs made minus number of frees made.
    // This works if clang tests for double/invalid frees.
    int mallocs;
    // how long is our history.
    int history_count;

    MemtestHistoryEntry history[MALLOC_HISTORY];
} MemtestData;

static MemtestHistoryEntry *memtest_history_find(MemtestData *data, void *ptr) {
    for (int i = 0; i < MALLOC_HISTORY && i < data->history_count; i++) {
        if (data->history[i].ptr == ptr) {
            return &data->history[i];
        }
    }
    return NULL;
}

void test_exit(int success) {
    if (!success) {
        printf("You have errors in your solution, please fix them.\n");
        printf("%s:error\n", KEY);
        exit(1);
    } else {
        printf("All test cases passed.\n");
        printf("%s:success\n", KEY);
        exit(0);
    }
}

/**
 * Returns a function pointer to the next function.
 */
void *get_next_function(char* name) {
    char *msg;
    void *next_function;
    dlerror();
    next_function = dlsym(RTLD_NEXT, name);
    if ((msg = dlerror()) != NULL) {
        fprintf(stderr, "dlsym failed: %s\n", msg);
        fflush(stderr);
        exit(1);
    }
    return next_function;
}

#ifdef PRELOADED

int had_label[1 << (8 * sizeof(char))];

void test_here(unsigned char label) {
    //printf("%s:%c:here\n", KEY, label);
    had_label[label] = 1;
}

int _preload_had_label(unsigned char label) {
    return had_label[label] != 0;
}

void _preload_reset_label(unsigned char label) {
    had_label[label] = 0;
}

static MemtestData *memtest_started = NULL;

void memtest_set_active(MemtestData *data) {
    memtest_started = data;
}

static int memtest_should_fail() {
    if (memtest_started) {
        if (memtest_started->succeding_calls == 0) {
            DEBUG("malloc should produce error.\n");
            return 1;
        } else if (memtest_started->succeding_calls > 0) {
            memtest_started->succeding_calls--;
        }
        DEBUG("Allow malloc. Calls left: %ld\n", memtest_started->succeding_calls);
    }
    return 0;
}

static void memtest_history_add(void *ptr, size_t size) {
    // mark as not-freed if already allocated
    if (memtest_started) {
        MemtestHistoryEntry *entry = memtest_history_find(memtest_started, ptr);
        if (!entry && memtest_started->history_count < MALLOC_HISTORY) {
            entry = &memtest_started->history[memtest_started->history_count];
            memtest_started->history_count++;
        }
        if (entry) {
            entry->ptr = ptr;
            entry->freed = 0;
            entry->size = size;
        }
    }
}

static void memtest_history_free(void *ptr) {
    MemtestHistoryEntry *entry = memtest_history_find(memtest_started, ptr);
    if (entry) {
        entry->freed = 1;
    }
}

void *malloc (size_t size) {
    static void *(*old_malloc) (size_t) = NULL;
    if (!old_malloc) {
        old_malloc = get_next_function("malloc");
    }
    if (memtest_should_fail()) {
        errno = ENOMEM;
        return NULL;
    }
    void *res = old_malloc(size);
    if (memtest_started) {
        memtest_history_add(res, size);
    }
    DEBUG("malloc(%d) => %p\n", (int) size, res);
    return res;
}

void *realloc (void *ptr, size_t size) {
    static void *(*old_realloc) (void *ptr, size_t size);
    if (!old_realloc) {
        old_realloc = get_next_function("realloc");
    }
    if (memtest_should_fail()) {
        errno = ENOMEM;
        return NULL;
    }
    void *res = old_realloc(ptr, size);
    DEBUG("realloc(%p, %d) => %p\n", ptr, (int) size, res);
    return res;
}

void free (void *ptr) {
    static void (*old_free) (void *);
    if (!old_free) {
        old_free = get_next_function("free");
    }
    if (memtest_started) {
        // we do not actually free the data.
        if (ptr != NULL) {
            memtest_history_free(ptr);
        }
        DEBUG("free(%p)\n", ptr);
    } else {
        old_free(ptr);
    }
}

void *memalign (size_t alignment, size_t size) {
    static void *(*old_memalign) (size_t, size_t);
    if (!old_memalign) {
        old_memalign = get_next_function("memalign");
    }
    if (memtest_should_fail()) {
        errno = ENOMEM;
        return NULL;
    }
    void *res = old_memalign(alignment, size);
    memtest_history_add(res, size);
    DEBUG("memalign(%d, %d) => %p\n", (int) alignment, (int) size, res);
    return res;
}

#else
int had_error = 0;

#define _test_failed() ({had_error = 1;})


#define test_failed_expected(type, should, is, is_str, file, line) { \
    printf("%s line %d: %s => Expected " type ", but got " type "\n", file, line, is_str, should, is); \
    _test_failed(); }

void _test_failed_message(char *message, char *file, int line) {
    printf("%s line %d: %s\n", file, line, message);
    _test_failed();
}

void test_start(char *filename) {
    printf("Starting to test %s...\n", filename);
}

void _test_equals_int64(int64_t is, int64_t should, char *is_str, char *file,
        int line) {
    if (is != should) {
        test_failed_expected("%"PRIu64, should, is, is_str, file, line);
    }
}

void _test_equals_int(int is, int should, char *is_str, char* file, int line) {
    if (is != should) {
        test_failed_expected("%d", should, is, is_str, file, line);
    }
}

void _test_equals_ptr(void *is, void *should, char *is_str, char* file, int line) {
    if (is != should) {
        test_failed_expected("%p", should, is, is_str, file, line);
    }
}

void _test_equals_string(char* is, char* should, char *is_str, char* file, int line) {
    if (strcmp(is, should) != 0) {
        test_failed_expected("\"%s\"", should, is, is_str, file, line);
    }
}

void _test_contains_chars(const char* is, const char* should, char* file, int line) {
    int c = 0;
    int timeout = 4069;
    while (should[c] && timeout) {
        if (!*is) {
            printf("%s line %d: Searching for %s, but could not find char %c (pos=%d)", file, line, is, is[c], c);
            had_error = 1;
            return;
        }
        if (*is == should[c]) {
            c++;
        }
        timeout--;
        is++;
    }
    if (!timeout) {
        printf("%s line %d: Compare string too long.", file, line);
        _test_failed();
    }
}

extern int __attribute__((weak)) _preload_had_label(unsigned char label);

extern int __attribute__((weak)) _preload_reset_label(unsigned char label);

int test_end() {
    test_exit(!had_error);
    return had_error;
}

void test_expect(unsigned char label) {
    //printf("%s:%c:expect\n", KEY, label);
    _preload_reset_label(label);
}

void test_expected(unsigned char label) {
    if (!_preload_had_label(label)) {
        had_error = 1;
        printf("Label %c not reached.\n", label);
    }
    //printf("%s:%c:expected\n", KEY, label);
    _preload_reset_label(label);
}

void test_expected_warn(unsigned char label, char *warnstr) {
    if (!_preload_had_label(label)) {
        printf("Warning: label %c not reached: %s\n", label, warnstr);
    }
    _preload_reset_label(label);
}

extern void __attribute__((weak)) memtest_set_active(MemtestData *data);

static MemtestData memtest_stuff;

void memtest_start_tracking(long allowedMallocs) {
    memtest_stuff.succeding_calls = allowedMallocs;
    memtest_stuff.mallocs = 0;
    memtest_stuff.history_count = 0;
    memtest_set_active(&memtest_stuff);
}

void memtest_end_tracking() {
    memtest_set_active(NULL);
}

int memtest_missing_free() {
    return memtest_missing_frees() > 0;
}

int memtest_missing_frees() {
    int missing = 0;
    for (int i = 0; i < MALLOC_HISTORY && i < memtest_stuff.history_count;
            i++) {
        if (!memtest_stuff.history[i].freed) {
            missing++;
        }
    }
    return missing;
}

int memtest_is_malloced(void *ptr) {
    MemtestHistoryEntry *entry = memtest_history_find(&memtest_stuff, ptr);
    return entry && !entry->freed;
}

size_t memtest_get_size(void *ptr) {
    MemtestHistoryEntry *entry = memtest_history_find(&memtest_stuff, ptr);
    return entry->size;
}

#endif
