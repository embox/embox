
	# Just a Makefile for the basic
	# operations through the test file


#paths
INCLUDE = ../include
PROGRAMS = ../programs

#compiler
CC = gcc

#write comment
CFLAGS = -Wall -Werror -g -I$(INCLUDE)

#files .o
OBJS = memccpy_test.o $(PROGRAMS)/embox_memccpy.o

#executable file
EXEC = memccpy_test


$(EXEC): $(OBJS)
	$(CC) -o $(EXEC) $(OBJS)

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC)