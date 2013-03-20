
USERMODE_DIR := src/arch/usermode86

SRCS := emvisor.c uservisor_base.c
OBJS := $(SRCS:%.c=%.o)

CC := gcc
CPPFLAGS := -I$(USERMODE_DIR)/include -DEMVISOR
CFLAGS := -g

emvisor : $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJS) : %.o : $(USERMODE_DIR)/%.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<
