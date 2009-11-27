EXEC = count_md5
SRC  = count.c md5.c
OBJS = $(SRC:%.c=%.o)
BIN_DIR = ../../bin
TARGET  = monitor_ram
OC_TOOL = sparc-elf-objcopy

.PHONY: all clean

all: $(EXEC) count

$(EXEC): $(OBJS)
	$(CC) $^ -o $@

.c.o:
	$(CC) -c -I. $<

count:
	$(OC_TOOL) -j .text $(BIN_DIR)/$(TARGET) imagetext.bin
	cat ../autoconf.h | sed 's/#define MD5_CHECKSUM "[0-9a-fA-F]*"/#define MD5_CHECKSUM "$(shell ./count_md5 -i imagetext.bin)"/' > copyautoconf.h
#	$(CC) -c $(SRC_DIR)/testsuites/checksum/test_checksum.c

clean:
	rm -f *.o $(EXEC) *.bin
