#include "testlib.h"
#include "message_queue.h"

// You called mq_send in sendMulTask correctly.
int main() {
    test_start("message_queue.c");

    test_expect('a');
    test_expect('b');
    test_expect('c');
    test_equals_int(sendMulTask(42, 43, 44), 0);

    test_expected('a');
    test_expected('b');
    test_expected('c');

    return test_end();
}
