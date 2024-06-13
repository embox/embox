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
#include <util/math.h>
#include <util/pretty_print.h>

static void print_help(void) {
	printf("Usage: block_dev_test [-hl] [-i iters] [-s block_num] [-n block_count] <block device name>\n");
	printf("\t-l\t\t\t\t: Print all available block and flash devices\n");
	printf("\t-i <iters>\t\t\t: Execute <iters> itertions of read/write\n");
	printf("\t-s <block_num>\t\t\t: <block_num> block number at which command should start at\n");
	printf("\t-n <block_count>\t\t: <block_count> blocks to be tested\n");
	printf("\t-m <multiple_block_count>\t: read/write <multiple_block_count> blocks in a single iteration\n\t\t\t\t\t  if <multiple_block_count> is not specified, default is taken as 4.\n");
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
			printf("%4" PRIdMAX " | %s\n", (uintmax_t)block_dev_id(bdevs[i]),
			    block_dev_name(bdevs[i]));
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
		if (!strcmp(block_dev_name(fdev->bdev), block_dev_name(bdev))) {
			return fdev;
		}
	}
	return NULL;
}

static int flash_dev_test(struct flash_dev *fdev, uint32_t s_block, uint32_t n_blocks, uint32_t m_blocks) {
#define FLASH_RW_LEN 256
	int offset;
	uint8_t rbuf[FLASH_RW_LEN];
	uint8_t wbuf[FLASH_RW_LEN];
	uint32_t blocks, total_blocks;

	total_blocks = flash_get_blocks_num(fdev);
	if (n_blocks) {
		blocks = min(s_block + n_blocks, total_blocks);
	} else {
		blocks = total_blocks;
	}

	for (int i = 0; i < FLASH_RW_LEN; i++) {
		wbuf[i] = 0x55;
	}

	if (s_block >= blocks) {
		printf("Starting block should be less than number of blocks\n");
		return -EINVAL;
	}

	/* Iterate over all flash blocks */
	for (uint32_t i = s_block; i < blocks; i += m_blocks) {

		for(uint32_t k = i; k < min(blocks, i + m_blocks); k++) {
			int blk_size = flash_get_block_size(fdev, k);

			flash_erase(fdev, k);

			offset = flash_get_offset_by_block(fdev, k);

			/* Write, then read back and check result */
			for (size_t j = 0; j < blk_size; j += FLASH_RW_LEN) {
				/* Clean rbuf first */
				memset(rbuf, 0x0, FLASH_RW_LEN);
				if (0 > flash_write(fdev, offset, wbuf, FLASH_RW_LEN)) {
					printf("Flash device test flash_write() failed\n");
					printf("  fdev=%s, block=%"PRIu32", offset within block=%zu\n",
							block_dev_name(fdev->bdev), k, j);
					return -1;
				}
				flash_read(fdev, offset, rbuf, FLASH_RW_LEN);
				if (0 != memcmp(wbuf, rbuf, FLASH_RW_LEN)) {
					printf("Flash device test failed:\n");
					printf("  fdev=%s, block=%"PRIu32", offset within block=%zu\n",
							block_dev_name(fdev->bdev), k, j);
					return -1;
				}
				offset += FLASH_RW_LEN;
			}
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

static int block_dev_test(struct block_dev *bdev, uint64_t s_block, uint64_t n_blocks, uint64_t m_blocks) {
	size_t blk_sz;
	int8_t *read_buf, *write_buf;
	uint64_t blocks, total_blocks;
	int err;

	blk_sz = bdev->block_size;
	if (blk_sz == 0) {
		printf("block size is zero, that probably shouldn't happen\n");
		return -1;
	}

	total_blocks = bdev->size / ((uint64_t) blk_sz);

	if (n_blocks) {
		blocks = min(s_block + n_blocks, total_blocks);
	} else {
		blocks = total_blocks;
	}

	read_buf = calloc(m_blocks, blk_sz);
	write_buf = calloc(m_blocks, blk_sz);

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

	if (s_block >= blocks) {
		printf("Starting block should be less than number of blocks\n");
		return -EINVAL;
	}

	for (uint64_t i = s_block; i < blocks; i += m_blocks) {
		printf("Testing %"PRIu64" (of %"PRIu64")\n", i, total_blocks);

		int out_of_range = 0;
		/**
		 * When writing multiple blocks, if the block we are supposed
		 * to write is outside the block range, then write only till
		 * the block range and ignore the rest
		 */
		if (i + m_blocks >= blocks) {
			m_blocks = blocks - i - 1;
			out_of_range = 1;
		}

		fill_buffer(write_buf, blk_sz * m_blocks);

		err = block_dev_write(bdev, (void *) write_buf, blk_sz * m_blocks, i);
		if (err < 0) {
			printf("Failed to write block #%"PRIu64"\n", i);
			goto free_buf;
		}

		err = block_dev_read(bdev, (void *) read_buf, blk_sz * m_blocks, i);
		if (err < 0) {
			printf("Failed to read block #%"PRIu64"\n", i);
			goto free_buf;
		}

		err = memcmp(read_buf, write_buf, blk_sz * m_blocks);
		if (err != 0) {
			printf("Write/read mismatch!\n");
			dump_buf(write_buf, blk_sz * m_blocks, "Write buffer");
			dump_buf(read_buf, blk_sz * m_blocks, "Read buffer");
			goto free_buf;
		}

		if (out_of_range) {
			break;
		}
	}

free_buf:
	free(read_buf);
	free(write_buf);

	return err;
}

static int dev_test(struct block_dev *bdev, uint64_t s_block, uint64_t n_blocks, uint64_t m_blocks) {
	struct flash_dev *fdev;

	fdev = get_flash_dev(bdev);
	if (fdev) {
		return flash_dev_test(fdev, s_block, n_blocks, m_blocks);
	} else {
		return block_dev_test(bdev, s_block, n_blocks, m_blocks);
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
			return -EIO;
		}

		md5_append(&state, read_buf, blk_sz);
	}

	/* Data beyond partition */
	for (uint64_t j = last_blk; j < blocks; j++) {
		err = block_dev_read(bdev, read_buf, blk_sz, j);
		if (err < 0) {
			printf("Failed to read block #%"PRIu64"\n", j);
			return -EIO;
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
			printf("%s\n", block_dev_name(b));
			if (b->block_size != blk_sz) {
				printf("Block size mismatch! %zi for %s; %zi for %s\n",
						blk_sz, block_dev_name(bdev), b->block_size, block_dev_name(b));
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
			block_dev_name(bdev));
	for (int i = 0; i < parts_n; i++) {
		blocks = parts[i]->size / ((uint64_t) blk_sz);

		printf("%s ... ", block_dev_name(parts[i]));
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
			block_dev_name(bdev));
	blocks = bdev->size / ((uint64_t) blk_sz);

	/* Skip first block because it contains MBR */
	for (int64_t j = 1; j < blocks; j++) {
		fill_buffer(write_buf, blk_sz);
		err = block_dev_write(bdev, (void *) write_buf, blk_sz, j);
		if (err < 0) {
			printf("Failed to write block #%"PRIu64" to %s\n",
					j, block_dev_name(bdev));
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
							j, block_dev_name(parts[i]));
					goto free_buf;
				}

				err = memcmp(read_buf, write_buf, blk_sz);
				if (err != 0) {
					printf("Write/read mismatch! %s block %"PRIu64"\n",
							block_dev_name(bdev), j);
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

		printf("%s ... ", block_dev_name(parts[i]));
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
							j, block_dev_name(parts[i]));
					goto free_buf;
				}
			} else if (err >= 0) {
				/* Outer writes should fail */
				printf("Block #%"PRIu64" is outside of %s, "
						"but it was written successfully\n",
						j,
						block_dev_name(parts[i]));
				goto free_buf;
			}
		}

		if (md5_outside_part(bdev, parts[i], digest_post, read_buf)) {
			goto free_buf;
		}

		if (memcmp(digest_pre, digest_post, sizeof(digest_pre))) {
			printf("Writes to %s change content outside of partition!\n",
					block_dev_name(parts[i]));
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

static int is_valid_argument(char *endptr, char *str) {
	if (endptr == NULL || *endptr != '\0') {
		printf("Invalid %s argument\n", str);
		exit(1);
	}
	return 0;
}

int main(int argc, char **argv) {
	char *endptr = NULL;
	int i, opt, iters = 1, test_partitions = 0, n_blocks_flag = 0, m_blocks_flag = 0;
	uint64_t s_block = 0, n_blocks = 0, m_blocks = 1;
	struct block_dev *bdev;

	if (argc < 2) {
		print_help();
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "hpli:s:n:m:"))) {
		switch (opt) {
			case 'p':
				test_partitions = 1;
				break;
			case 'l':
				print_block_devs();
				return 0;
			case 'i':
				iters = strtol(optarg, &endptr, 0);
				is_valid_argument(endptr, "<iters>");
				break;
		        case 's':
				s_block = strtoll(optarg, &endptr, 0);
				is_valid_argument(endptr, "<block_num>");
				break;
		        case 'n':
				n_blocks = strtoll(optarg, &endptr, 0);
				is_valid_argument(endptr, "<block_count>");
				n_blocks_flag = 1;
				break;
		        case 'm':
				m_blocks = strtoll(optarg, &endptr, 0);
				is_valid_argument(endptr, "<multiple_block_count>");
				m_blocks_flag = 1;
				break;
			case 'h':
				print_help();
				return 0;
			default:
				print_help();
				return -ENOTSUP;
		}
	}

	if (m_blocks_flag) {
		if (!m_blocks) {
			printf("Multiple blocks count argument value is not provided\n");
			return -EINVAL;
		}
	}

	if (n_blocks_flag) {
		if (!n_blocks) {
			printf("Block count argument value should be greater than zero\n");
			return -EINVAL;
		}
	}

	bdev = block_dev_find(argv[argc - 1]);
	if (!bdev) {
		printf("Block device \"%s\" not found\n", argv[argc - 1]);
		return -EINVAL;
	}

	if (test_partitions) {
		return part_test(bdev);
	}

	printf("Starting block device test (iters = %d)...\n", iters);
	for (i = 0; i < iters; i++) {
		printf("iter %d...\n", i);
		if (dev_test(bdev, s_block, n_blocks, m_blocks) < 0) {
			printf("FAILED\n");
			return -1;
		}
	}
	printf("OK\n");

	return 0;
}
