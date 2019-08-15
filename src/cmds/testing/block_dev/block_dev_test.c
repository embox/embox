/**
 * @file 
 * @brief Simple test for r/w operation on block device
 * 
 * @author Alexander Kalmuk
 * @date 15.08.2019
 */

#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <framework/mod/options.h>
#include <drivers/flash/flash.h>
#include <module/embox/driver/block_common.h>

#define MAX_BDEV_QUANTITY \
	OPTION_MODULE_GET(embox__driver__block_common, NUMBER, dev_quantity)

static void print_help(void) {
	printf("Usage: block_dev_test [-hl] [-i iters] <block device name>\n");
	printf("\t-l        : Print all available block and flash devices\n");
	printf("\t-i <iters>: Execute <iters> itertions of read/write\n");
}

static void print_block_devs(void) {
	int i;
	struct block_dev **bdevs;

	bdevs = get_bdev_tab();
	assert(bdevs);

	printf("Block devices:\n");
	printf("  id | name\n");

	for (i = 0; i < MAX_BDEV_QUANTITY; i++) {
		if (bdevs[i]) {
			printf("%4d | %s\n", bdevs[i]->id, bdevs[i]->name);
		}
	}
	printf("\n");
}

static struct flash_dev *get_flash_dev(struct block_dev *bdev) {
	int i = 0;
	struct flash_dev *fdev;

	while (1) {
		fdev = flash_by_id(i++);
		if (!fdev) {
			break;
		}
		if (!strcmp(fdev->bdev->name, bdev->name)) {
			return fdev;
		}
	}
	return NULL;
}

static int flash_dev_test(struct flash_dev *fdev) {
#define FLASH_RW_LEN 256
	int i, j;
	int offset;
	uint8_t rbuf[FLASH_RW_LEN];
	uint8_t wbuf[FLASH_RW_LEN];

	for (i = 0; i < FLASH_RW_LEN; i++) {
		wbuf[i] = 0x55;
	}

	/* Iterate over all flash blocks */
	for (i = 0; i < fdev->block_info[0].blocks; i++) {
		flash_erase(fdev, i);

		offset = i * fdev->block_info[0].block_size;

		/* Write, then read back and check result */
		for (j = 0; j < fdev->block_info[0].block_size; j += FLASH_RW_LEN) {
			/* Clean rbuf first */
			memset(rbuf, 0x0, FLASH_RW_LEN);
			flash_write(fdev, offset, wbuf, FLASH_RW_LEN);
			flash_read(fdev, offset, rbuf, FLASH_RW_LEN);
			if (0 != memcmp(wbuf, rbuf, FLASH_RW_LEN)) {
				printf("Flash device test failed:\n");
				printf("  fdev=%s, block=%d, offset within block=%d\n",
					fdev->bdev->name, i, j);
				return -1;
			}
			offset += FLASH_RW_LEN;
		}
	}
	return 0;
#undef FLASH_RW_LEN
}

static int block_dev_test(const char *name) {
	struct block_dev *bdev;
	struct flash_dev *fdev;

	bdev = block_dev_find(name);
	if (!bdev) {
		printf("Block device \"%s\" not found\n");
		return -1;
	}
	fdev = get_flash_dev(bdev);
	if (!fdev) {
		printf("Only flash block devices are supported\n");
		return -1;
	}
	return flash_dev_test(fdev);
}

int main(int argc, char **argv) {
	int opt;
	int i, iters = 1;

	if (argc < 2) {
		print_help();
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "hli:"))) {
		switch (opt) {
			case 'l':
				print_block_devs();
				return 0;
			case 'i':
				iters = strtol(argv[optind], NULL, 0);
				break;
			case 'h':
			default:
				print_help();
				return 0;
		}
	}

	printf("Starting block device test (iters = %d)...\n", iters);
	for (i = 0; i < iters; i++) {
		printf("iter %d...\n", i);
		if (block_dev_test(argv[argc - 1]) < 0) {
			printf("FAILED\n");
			return -1;
		}
	}
	printf("OK\n");
	return 0;
}
