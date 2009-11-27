EXEC = count_md5
SRC  = count.c md5.c
OBJS = $(SRC:%.c=%.o)
TARGET  = monitor_ram
CC = gcc

.PHONY: all clean

all: $(EXEC) count

$(EXEC): $(OBJS)
	@$(CC) $^ -o $@

.c.o:
	@$(CC) -c -I. $<

count:
	@$(OC_TOOL) -j .text $(BIN_DIR)/$(TARGET) imagetext.bin 2> /dev/null
	@cat ../autoconf.h | sed 's/#define MD5_CHECKSUM "[0-9a-fA-F]*"/#define MD5_CHECKSUM "$(shell ./count_md5 -i imagetext.bin)"/' > copyautoconf.h
	@mv -f copyautoconf.h ../autoconf.h
#	$(CC) -c $(SRC_DIR)/testsuites/checksum/test_checksum.c

clean:
	@rm -f *.o $(EXEC) *.bin
