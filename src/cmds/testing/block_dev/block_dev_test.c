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

#include <drivers/block_dev.h>
#include <drivers/flash/flash.h>
#include <lib/crypt/md5.h>
#include <util/pretty_print.h>

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
		printf("block size is zero, that probably shouldn't happen\n");
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
		printf("Testing %"PRIu64" (of %"PRIu64")\n", i, blocks);

		fill_buffer(write_buf, blk_sz);

		err = block_dev_write(bdev, (void *) write_buf, blk_sz, i);
		if (err < 0) {
			printf("Failed to write block #%"PRIu64"\n", i);
			goto free_buf;
		}

		err = block_dev_read(bdev, (void *) read_buf, blk_sz, i);
		if (err < 0) {
			printf("Failed to read block #%"PRIu64"\n", i);
			goto free_buf;
		}

		err = memcmp(read_buf, write_buf, blk_sz);
		if (err != 0) {
			printf("Write/read mismatch!\n");
			dump_buf(write_buf, blk_sz, "Write buffer");
			dump_buf(read_buf, blk_sz, "Read buffer");
			goto free_buf;
		}
	}

free_buf:
	free(read_buf);
	free(write_buf);

	return err;
}

static int dev_test(struct block_dev *bdev) {
	struct flash_dev *fdev;

	fdev = get_flash_dev(bdev);
	if (fdev) {
		return flash_dev_test(fdev);
	} else {
		return block_dev_test(bdev);
	}
}

static int md5_outside_part(struct block_dev *bdev,
		struct block_dev *part,
		md5_byte_t *digest,
		void *read_buf) {
	md5_state_t state;
	int err;
	int blk_sz = part->block_size;
	uint64_t blocks = bdev->size / ((uint64_t) blk_sz);

	md5_init(&state);

	uint64_t last_blk = part->start_offset + part->size / blk_sz;

	/* Data before partition */
	for (uint64_t j = 0; j < part->start_offset; j++) {
		err = block_dev_read(bdev, read_buf, blk_sz, j);
		if (err < 0) {
			printf("Failed to read block #%"PRIu64"\n", j);
			return -1;
		}

		md5_append(&state, read_buf, blk_sz);
	}

	/* Data beyond partition */
	for (uint64_t j = last_blk; j < blocks; j++) {
		err = block_dev_read(bdev, read_buf, blk_sz, j);
		if (err < 0) {
			printf("Failed to read block #%"PRIu64"\n", j);
			return -1;
		}

		md5_append(&state, read_buf, blk_sz);
	}

	md5_finish(&state, digest);
	return 0;
}

#define TEST_PARTS 4
static int part_test(struct block_dev *bdev) {
	struct block_dev *parts[TEST_PARTS] = { NULL };
	int8_t *read_buf, *write_buf;
	int parts_n = 0, err, blocks;
	size_t blk_sz;

	blk_sz = bdev->block_size;
	if (blk_sz == 0) {
		printf("block size is zero, that probably shouldn't happen\n");
		return 0;
	}

	printf("Looking for partitions...\n");
	for (int i = 0; i < block_dev_max_id(); i++) {
		struct block_dev *b = block_dev_by_id(i);

		if (b && b->parent_bdev == bdev) {
			printf("%s\n", b->name);
			if (b->block_size != blk_sz) {
				printf("Block size mismatch! %d for %s; %d for %s\n",
						blk_sz, bdev->name, b->block_size, b->name);
				return 0;
			}

			if (parts_n == TEST_PARTS) {
				printf("Too many partitions, test only first %d\n",
						TEST_PARTS);
				break;
			}

			parts[parts_n++] = b;
		}
	}

	if (parts[0] == NULL) {
		printf("No partitions found. Run without '-p' flag "
				"for simple block device test.\n");
		return 0;
	}

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

		return 0;
	}

	printf("Step 1/3: Check if partitions writes also change %s content\n",
			bdev->name);
	for (int i = 0; i < parts_n; i++) {
		blocks = parts[i]->size / ((uint64_t) blk_sz);

		printf("%s ... ", parts[i]->name);
		fflush(stdout);

		for (int64_t j = 0; j < blocks; j++) {
			fill_buffer(write_buf, blk_sz);
			err = block_dev_write(parts[i], (void *) write_buf, blk_sz, j);
			if (err < 0) {
				printf("Failed to write block #%"PRIu64"\n", j);
				goto free_buf;
			}

			err = block_dev_read(bdev,
					(void *) read_buf,
					blk_sz,
					j + parts[i]->start_offset);
			if (err < 0) {
				printf("Failed to read block #%"PRIu64"\n", j);
				goto free_buf;
			}

			err = memcmp(read_buf, write_buf, blk_sz);
			if (err != 0) {
				printf("Write/read mismatch!\n");
				dump_buf(write_buf, blk_sz, "Write buffer");
				dump_buf(read_buf, blk_sz, "Read buffer");
				goto free_buf;
			}
		}
		printf("OK\n");
	}

	printf("Step 2/3: Check if %s writes also change partitions content\n",
			bdev->name);
	blocks = bdev->size / ((uint64_t) blk_sz);

	/* Skip first block because it contains MBR */
	for (int64_t j = 1; j < blocks; j++) {
		fill_buffer(write_buf, blk_sz);
		err = block_dev_write(bdev, (void *) write_buf, blk_sz, j);
		if (err < 0) {
			printf("Failed to write block #%"PRIu64" to %s\n",
					j, bdev->name);
			goto free_buf;
		}

		for (int i = 0; i < parts_n; i++) {
			uint64_t last_blk = parts[i]->start_offset +
				parts[i]->size / blk_sz;

			if (parts[i]->start_offset <= j && last_blk > j) {
				err = block_dev_read(parts[i],
						(void *) read_buf,
						blk_sz,
						j - parts[i]->start_offset);
				if (err < 0) {
					printf("Failed to read block #%"PRIu64" from %s\n",
							j, parts[i]->name);
					goto free_buf;
				}

				err = memcmp(read_buf, write_buf, blk_sz);
				if (err != 0) {
					printf("Write/read mismatch! %s block %"PRIu64"\n",
							bdev->name, j);
					dump_buf(write_buf, blk_sz, "Write buffer");
					dump_buf(read_buf, blk_sz, "Read buffer");
					goto free_buf;
				}
			}
		}
	}
	printf("OK\n");

	printf("Step 3/3: Check if partition writes do not affect other partitions\n");
	blocks = bdev->size / ((uint64_t) blk_sz);
	for (int i = 0; i < parts_n; i++) {
		uint64_t last_blk = parts[i]->size / blk_sz;
		md5_byte_t digest_pre[16], digest_post[16];

		printf("%s ... ", parts[i]->name);
		fflush(stdout);

		if (md5_outside_part(bdev, parts[i], digest_pre, read_buf)) {
			goto free_buf;
		}

		/* Loop border is taken too large on purpose */
		for (uint64_t j = 0; j < blocks; j++) {
			fill_buffer(write_buf, blk_sz);
			err = block_dev_write(parts[i], (void *) write_buf, blk_sz, j);

			if (j < last_blk) {
				/* Inner writes should succeed */
				if (err < 0) {
					printf("Failed to write block #%"PRIu64" from %s\n",
							j, parts[i]->name);
					goto free_buf;
				}
			} else if (err >= 0) {
				/* Outer writes should fail */
				printf("Block #%"PRIu64" is outside of %s, "
						"but it was written successfully\n",
						j,
						parts[i]->name);
				goto free_buf;
			}
		}

		if (md5_outside_part(bdev, parts[i], digest_post, read_buf)) {
			goto free_buf;
		}

		if (memcmp(digest_pre, digest_post, sizeof(digest_pre))) {
			printf("Writes to %s change content outside of partition!\n",
					parts[i]->name);
			goto free_buf;
		}
		printf("OK\n");
	}

	free(read_buf);
	free(write_buf);

	return 0;

free_buf:
	printf("FAILED!\n");
	free(read_buf);
	free(write_buf);

	return err;
}

int main(int argc, char **argv) {
	int opt;
	int i, iters = 1, test_partitions = 0;
	struct block_dev *bdev;

	if (argc < 2) {
		print_help();
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "hpli:"))) {
		switch (opt) {
			case 'p':
				test_partitions = 1;
				break;
			case 'l':
				print_block_devs();
				return 0;
			case 'i':
				iters = strtol(optarg, NULL, 0);
				break;
			case 'h':
			default:
				print_help();
				return 0;
		}
	}

	bdev = block_dev_find(argv[argc - 1]);
	if (!bdev) {
		printf("Block device \"%s\" not found\n", argv[argc - 1]);
		return 0;
	}

	if (test_partitions) {
		return part_test(bdev);
	}

	printf("Starting block device test (iters = %d)...\n", iters);
	for (i = 0; i < iters; i++) {
		printf("iter %d...\n", i);
		if (dev_test(bdev) < 0) {
			printf("FAILED\n");
			return -1;
		}
	}
	printf("OK\n");

	return 0;
}
