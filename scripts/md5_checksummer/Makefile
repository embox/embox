TARGET = monitor
OC_TOOL = sparc-elf-objcopy

all:count_md5
	echo done

count.o: count.c
	gcc -c -o $@ $<

md5.o:
	gcc -I$(INCLUDE_DIR) -c $(SRC_DIR)/lib/crypt/md5.c

count_md5:  count.o md5.o
	gcc -o count_md5 count.o md5.o
	$(OC_TOOL) -O binary -j .text $(BIN_DIR)/$(TARGET)_ram imagetext.bin
	cat ../autoconf.h | sed 's/#define MD5_CHECKSUM ".*"/#define MD5_CHECKSUM "$(shell ./count_md5 -i imagetext.bin)"/' > copyautoconf.h
#	mv -f copyautoconf.h ../autoconf.h
#	$(CC) -c $(SRC_DIR)/testsuites/checksum/test_checksum.c

clean:
	rm -f *.o
