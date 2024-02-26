CC=     gcc
LD=     ld
CFLAGS= -g -W -Wall -Werror -std=c99
TARGET= malloc
SRC=    malloc.c testlib.c main.c
OBJ=    $(SRC:.c=.o)

all:
	@echo "Nothing to do here"

showsrc:
	@echo sim-paging.py traces.txt mempattern-1.png mempattern-2.png simresult-1.png simresult-2.png

zip:
	cd ..; ./makezip.py
