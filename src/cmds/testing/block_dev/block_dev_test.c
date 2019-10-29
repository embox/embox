/**
 * @file
 * @brief Simple test for r/w operation on block device
 *
 * @author Alexander Kalmuk
 * @date 15.08.2019
 */

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <framework/mod/options.h>
#include <drivers/flash/flash.h>
#include <module/embox/driver/block_common.h>
#include <util/pretty_print.h>

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

static void fill_buffer(int8_t *buf, size_t cnt) {
	while (cnt) {
		long int r = random();
		size_t step = sizeof(r);

		if (cnt < step) {
			break;
		}

		memcpy(buf, &r, step);

		buf += step;
		cnt -= step;
	}
}

static void dump_buf(int8_t *buf, size_t cnt, char *fmt) {
	char msg[256];
	size_t step = pretty_print_row_len();

	printf("============================\n");
	printf("%s\n", fmt);
	printf("============================\n");
	while (cnt) {
		pretty_print_row(buf, cnt, msg);

		printf("%s\n", msg);

		if (cnt < step) {
			cnt = 0;
		} else {
			cnt -= step;
		}

		buf += step;
	}
	printf("============================\n");
}

static int block_dev_test(struct block_dev *bdev) {
	size_t blk_sz;
	int8_t *read_buf, *write_buf;
	uint64_t blocks;
	int err;

	blk_sz = bdev->block_size;
	if (blk_sz == 0) {
		printf("block size is zero, that's probably shouldn't happen\n");
		return -1;
	}

	blocks = bdev->size / ((uint64_t) blk_sz);
	read_buf = malloc(blk_sz);
	write_buf = malloc(blk_sz);

	if (read_buf == NULL || write_buf == NULL) {
		printf("Failed to allocate memory for buffer!\n");

		if (read_buf != NULL) {
			free(read_buf);
		}

		if (write_buf != NULL) {
			free(write_buf);
		}

		return -ENOMEM;
	}

	for (uint64_t i = 0; i < blocks; i++) {
		fill_buffer(write_buf, blk_sz);

		err = block_dev_write(bdev, (void *) write_buf, blk_sz, i);
		if (err < 0) {
			printf("Failed to write block #%"PRIu64"\n", i);
			return err;
		}

		err = block_dev_read(bdev, (void *) read_buf, blk_sz, i);
		if (err < 0) {
			printf("Failed to read block #%"PRIu64"\n", i);
			return err;
		}

		err = memcmp(read_buf, write_buf, blk_sz);
		if (err != 0) {
			printf("Write/read mismatch!\n");
			dump_buf(write_buf, blk_sz, "Write buffer");
			dump_buf(read_buf, blk_sz, "Read buffer");
			return -1;
		}
	}

	free(read_buf);
	free(write_buf);

	return 0;
}

static int dev_test(const char *name) {
	struct block_dev *bdev;
	struct flash_dev *fdev;

	bdev = block_dev_find(name);
	if (!bdev) {
		printf("Block device \"%s\" not found\n", name);
		return -1;
	}

	fdev = get_flash_dev(bdev);
	if (fdev) {
		return flash_dev_test(fdev);
	} else {
		return block_dev_test(bdev);
	}
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
		if (dev_test(argv[argc - 1]) < 0) {
			printf("FAILED\n");
			return -1;
		}
	}
	printf("OK\n");
	return 0;
}
