#include "print.h"
#include <stdio.h>

void print_line(int64_t number, char *string)
{
    (void) number;
    (void) string;
    // Add code here.
    // we used PRId64 to format 64-bit integer as the assignment asked for, and we used %s to format the string
    printf("%" PRId64 " %s\n", number, string);
}
