# Compiler and flags
CC = gcc
CXX = g++
CFLAGS = -I$(PKG_INSTALL_DIR)/include -I$(EMBOX_INCLUDE) -Wall -Wextra -g
LDFLAGS =

# Source files
SRCS = etl_test.c
OBJS = $(SRCS:.c=.o)

# Target executable
TARGET = etl_tests

# Default target
all: $(TARGET)

# Link step
$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

# Compile step
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean objects and binary
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
