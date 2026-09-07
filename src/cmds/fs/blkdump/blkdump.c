/**
 * @file
 * @brief Dump blocks of a block device in hex
 *
 * @date 05.09.2026
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BLK_SIZE  512
#define ROW_SIZE  16

static void print_usage(void) {
	printf("Usage: blkdump [-s lba] [-n count] dev\n");
}

static void dump_block(int fd, unsigned long lba, unsigned char *buf) {
	uint32_t sum = 0;
	int i, j;
	int rc;

	rc = read(fd, buf, BLK_SIZE);
	if (rc != BLK_SIZE) {
		printf("block %lu: short read (%d)\n", lba, rc);
		return;
	}

	for (i = 0; i < BLK_SIZE; i += ROW_SIZE) {
		printf("%08lx:", lba * BLK_SIZE + i);

		for (j = 0; j < ROW_SIZE; j++) {
			printf(" %02x", buf[i + j]);
		}
		printf("\n");
	}

	for (i = 0; i < BLK_SIZE; i += 4) {
		sum += (uint32_t) buf[i] | ((uint32_t) buf[i + 1] << 8) |
				((uint32_t) buf[i + 2] << 16) |
				((uint32_t) buf[i + 3] << 24);
	}

	printf("%08lx: checksum %08x\n", lba, sum);
}

int main(int argc, char **argv) {
	unsigned char buf[BLK_SIZE];
	unsigned long lba = 0;
	long count = 1;
	int opt;
	int fd;

	while (-1 != (opt = getopt(argc, argv, "hs:n:"))) {
		switch (opt) {
		case 's':
			lba = strtoul(optarg, NULL, 0);
			break;
		case 'n':
			count = strtol(optarg, NULL, 0);
			break;
		case 'h':
		default:
			print_usage();
			return 0;
		}
	}

	if (optind != argc - 1) {
		print_usage();
		return -EINVAL;
	}

	fd = open(argv[optind], O_RDONLY);
	if (fd < 0) {
		printf("blkdump: %s: open failed\n", argv[optind]);
		return -EACCES;
	}

	if (lseek(fd, lba * BLK_SIZE, SEEK_SET) < 0) {
		printf("blkdump: seek failed\n");
		close(fd);
		return -EINVAL;
	}

	for (; count > 0; count--, lba++) {
		dump_block(fd, lba, buf);
	}

	close(fd);

	return 0;
}
