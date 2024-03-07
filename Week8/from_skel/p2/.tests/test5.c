#include "testlib.h"
#include "message_queue.h"

// You call mq_open in runServer correctly.
int main() {
    test_start("message_queue.c");

    test_expect('a');
    //test_expect('b');
    test_expect('c');
    test_expect('d');
    test_expect('e');
    test_equals_int(runServer(), -1);
    test_expected_warn('a', "You are not using the default queue name");
    //test_expected('b');
    test_expected('c');
    test_expected('d');
    test_expected('e');

    return test_end();
}
