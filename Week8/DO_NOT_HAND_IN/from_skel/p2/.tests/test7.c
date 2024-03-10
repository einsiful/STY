#include "testlib.h"
#include "message_queue.h"

// You close and unlink the queue correctly.
int main() {
    test_start("message_queue.c");
    mq_unlink("/simple_calculator");

    test_expect('a');
    test_expect('b');
    test_equals_int(runServer(), 0);
    test_expected('a');
    test_expected('b');
    mq_unlink("/simple_calculator");

    return test_end();
}
