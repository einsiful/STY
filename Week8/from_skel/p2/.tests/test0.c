#include "testlib.h"
#include "message_queue.h"

// You called mq_open in startClient().
int main() {
    test_start("message_queue.c");

    test_expect('a');
    test_expect('b');
    test_equals_int(startClient(), 42);

    test_expected_warn('a', "You are not using the default queue name");
    test_expected('b');

    return test_end();
}
