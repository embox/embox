HOST_SRCS := $(wildcard *.c)
HOST_OBJS := $(addprefix $(MOD_BUILD_DIR)/,$(HOST_SRCS:.c=.o))

$(BUILD) : $(HOST_OBJS)
	@touch $@

CC := $(EMBOX_CROSS_COMPILE)gcc
CFLAGS := $(filter -g% -f% -m% -O% -G% -E%,$(EMBOX_CFLAGS))
CPPFLAGS := -I../include/kernel/
$(MOD_BUILD_DIR)/%.o : %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
