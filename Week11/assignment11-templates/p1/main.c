#include "testlib.h"
#include "filesystem.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define HELLO_SIZE 1

char *files[] = { "en.txt", "is.txt", "chain", "endian", "secret", "hello", NULL };

int main()
{
    test_start("filesystem.c");
    char buf[1024];

    printf("Opening file system...\n");
    FileSystem *fs = initFileSystem("test.image");
    if (fs == NULL) {
        test_failed_message("No file system returned.");
        return test_end();
    }

    // Test that you loaded the right file system.
    test_equals_int64(fs->header->rootDirectorySize, 6 * sizeof (DirectoryEntry));

    printf("Reading some files:\n");
    // We want to read the file "hello" and some other files.

    for (char **fileName = files; *fileName != NULL; fileName++) {
        printf("\n");
        char *dir = NULL;
        if(fileName == files+5) {
            dir = "user";
        }
        printf("Reading file %s\n", *fileName);
        OpenFileHandle *file = openFile(fs, dir, *fileName);

        if (file == NULL) {
            test_failed_message("File is not there.");
        } else {
            int n = readFile(file, buf, file->length);
            test_equals_int64(n, file->length);

            // Write the file content to stdout
            printf("File contents: ");
            fflush(stdout);
            write(1, buf, n);
            fflush(stdout);
            printf("<EOF>\n");
            closeFile(file);
        }
    }

    return test_end();
}
