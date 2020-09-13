#makefile for compiling the malezadanie project
CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -O2
RM=/bin/rm -f

all: quantization

#create the object files

main.o: structures.h read.h execute_command.h main.c

	$(CC) $(CFLAGS) -c main.c

read.o: structures.h check_command.h read.h execute_command.h read.c

	$(CC) $(CFLAGS) -c read.c

execute_command.o: structures.h execute_command.h execute_command.c

	$(CC) $(CFLAGS) -c execute_command.c

check_command.o: check_command.h check_command.c

	$(CC) $(CFLAGS) -c check_command.c

#create the executable
quantization: main.o read.o execute_command.o check_command.o

	$(CC) $(CFLAGS) -o quantization main.o read.o execute_command.o check_command.o

clean:

	$(RM) *.o main
	$(RM) *.o read
	$(RM) *.o execute_command
	$(RM) *.o check_command
	$(RM) quantization
