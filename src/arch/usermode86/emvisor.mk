
USERMODE_DIR := src/arch/usermode86

SRCS := emvisor.c uservisor_base.c
OBJS := $(SRCS:%.c=%.o)
TARGET := emvisor

CC := gcc
CPPFLAGS := -DEMVISOR
CFLAGS := -g
cppflags = $(CPPFLAGS)

uservisor_base.o : cppflags = $(CPPFLAGS) -isystem $(USERMODE_DIR)/include

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJS) : %.o : $(USERMODE_DIR)/%.c
	$(CC) -c $(cppflags) $(CFLAGS) -o $@ $<

clean :
	rm $(TARGET) $(OBJS)
