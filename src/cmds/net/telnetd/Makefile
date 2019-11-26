CFLAGS := $(CFLAGS)
CFLAGS += -g -O0 -DTELNETD_FOR_LINUX

objs := telnetd.o telnet_util.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

telnetd: $(objs)
	$(CC) -o $@ $^ $(CFLAGS)

all: telnetd
