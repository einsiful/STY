#include "testlib.h"
#include "message_queue.h"

// You called mq_close in stopClient correctly.
int main() {
    test_start("message_queue.c");

    test_expect('a');
    stopClient(42);
    test_expected('a');

    return test_end();
}
