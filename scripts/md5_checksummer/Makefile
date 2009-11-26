TARGET = monitor
OC_TOOL = sparc-elf-objcopy


all:count_md5
	echo done

count.o: count.c
	gcc -c -o $@ $<

md5.o:
	gcc -I$(INCLUDE_DIR) -c $(SRC_DIR)/lib/crypt/md5.c -o $@

count_md5:  count.o md5.o
	gcc -o count_md5 count.o md5.o
	$(OC_TOOL) -j .text $(BIN_DIR)/$(TARGET) imagetext.bin
	count_md5
#	$(CC) -c $(SRC_DIR)/testsuites/checksum/test_checksum.c
