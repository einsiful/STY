#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Base64 code from https://github.com/joedf/base64.c (copy of base64.h/base64.c included in the template)
#include "base64.h"

// Do not change this
#define LOGFILENAME "STY-P3-p1-logfile.txt"

void usage_exit(const char *argv[])
{
	printf("Usage: %s [-b] <logmessage>\n"
	       "  this program writes the log message from the command line (plus timestamp) into a log file\n"
	       "  log file name is %s\n"
	       "  if parameter -b is used: log message is base64 encoded and will be decoded before writing to log\n",
	       argv[0], LOGFILENAME);
	exit(1);
}

int main(int argc, const char *argv[])
{
	int base64 = 0;
	if (argc <= 1) {
		usage_exit(argv);
	}
	if (argc > 2) {
		if (strcmp(argv[1], "-b") == 0 && argc == 3)
			base64 = 1;
		else
			usage_exit(argv);
	}
	char filename[255];
	char logmsg[100];

	//Get the user 's home directory
	const char *home_dir = getenv("HOME");
	if (!home_dir) {
		perror("getenv() error");
		exit(1);
	}
	//Concatenate it with LOGFILENAME
	strcpy(filename, home_dir);
	strcat(filename, "/");
	strcat(filename, LOGFILENAME);

	//Get the time stamp(put at the start of the log message)
	time_t current_time;
	time(&current_time);
	strftime(logmsg, 100, "%Y-%m-%d %H:%M:%S", localtime(&current_time));
	char *ptr = logmsg + strlen(logmsg);
	*(ptr++) = ':';
	*(ptr++) = ' ';
	//Append the message from the command line
	if (base64)
		b64_decode((const unsigned char *)argv[2], strlen(argv[2]), (unsigned char *)ptr);
	else
		strcpy(ptr, argv[1]);

	//Append to log file(or create it if it does not exist)
	printf("Logging '%s' to '%s'\n", logmsg, filename);
	FILE *f = fopen(filename, "a+");
	if (!f) {
		perror("Error opening/creating file");
		exit(1);
	}
	fprintf(f, "%s\n", logmsg);
	fclose(f);

	printf("Debug: %p %p %ld\n", filename, logmsg, filename - logmsg);
	return 0;
}
