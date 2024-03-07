#include "testlib.h"
#include "message_queue.h"

// Your server exits when receiving an exit command.
int main() {
    test_start("message_queue.c");
    mq_unlink("/simple_calculator");

    test_expect('a');
    test_equals_int(runServer(), 0);
    test_expected('a');
    mq_unlink("/simple_calculator");

    return test_end();
}
