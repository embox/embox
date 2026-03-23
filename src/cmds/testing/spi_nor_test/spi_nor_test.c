/**
 * @file spi_nor_test.c
 * @brief Shell command to smoke-test the SPI NOR flash driver.
 *
 * Usage:
 *   spi_nor_test            full erase+write+verify on first flash device
 *   spi_nor_test -r <off>   hex-dump 64 bytes at <offset>
 *   spi_nor_test -e <blk>   erase block <blk>
 *   spi_nor_test -w <off>   write+verify 16-byte pattern at <offset>
 *
 * Placement: src/cmds/testing/spi_nor_test/spi_nor_test.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>

#include <drivers/flash/flash.h>

#define TEST_BLOCK    0
#define TEST_OFFSET   0
#define DUMP_LEN      64
#define PAT_LEN       16
#define PAT_BASE      0xA5u

static void hexdump(const uint8_t *buf, size_t len, uint32_t base) {
	size_t i;
	for (i = 0; i < len; i++) {
		if ((i & 15) == 0) {
			printf("\n  %06" PRIx32 ":", (uint32_t)(base + i));
		}
		printf(" %02x", buf[i]);
	}
	printf("\n");
}

static int do_read(struct flash_dev *dev, uint32_t off) {
	uint8_t buf[DUMP_LEN];
	int ret = dev->drv->flash_read(dev, off, buf, sizeof(buf));
	if (ret < 0) {
		fprintf(stderr, "spi_nor_test: read failed (%d)\n", ret);
		return ret;
	}
	printf("Read %d bytes at 0x%x:", DUMP_LEN, off);
	hexdump(buf, DUMP_LEN, off);
	return 0;
}

static int do_erase(struct flash_dev *dev, uint32_t blk) {
	int ret;
	printf("Erasing block %u (offset 0x%x)...\n",
	       blk, blk * dev->block_info[0].block_size);
	ret = dev->drv->flash_erase_block(dev, blk);
	if (ret < 0) {
		fprintf(stderr, "spi_nor_test: erase failed (%d)\n", ret);
		return ret;
	}
	printf("Erase OK\n");
	return 0;
}

static int do_write(struct flash_dev *dev, uint32_t off) {
	uint8_t wbuf[PAT_LEN], rbuf[PAT_LEN];
	int i, ret;

	for (i = 0; i < PAT_LEN; i++) {
		wbuf[i] = (uint8_t)(PAT_BASE ^ (uint8_t)i);
	}

	printf("Writing %d-byte pattern at 0x%x...\n", PAT_LEN, off);
	ret = dev->drv->flash_program(dev, off, wbuf, PAT_LEN);
	if (ret < 0) {
		fprintf(stderr, "spi_nor_test: write failed (%d)\n", ret);
		return ret;
	}

	ret = dev->drv->flash_read(dev, off, rbuf, PAT_LEN);
	if (ret < 0) {
		fprintf(stderr, "spi_nor_test: readback failed (%d)\n", ret);
		return ret;
	}

	if (memcmp(wbuf, rbuf, PAT_LEN) != 0) {
		fprintf(stderr, "spi_nor_test: VERIFY FAILED\n");
		printf("  written:"); hexdump(wbuf, PAT_LEN, off);
		printf("  read:   "); hexdump(rbuf, PAT_LEN, off);
		return -EIO;
	}
	printf("Write+verify OK\n");
	return 0;
}

static int do_full_test(struct flash_dev *dev) {
	int ret;
	printf("=== SPI NOR smoke test ===\n");
	printf("Device: %u KiB, %u blocks x %u KiB\n",
	       dev->size / 1024,
	       dev->block_info[0].blocks,
	       dev->block_info[0].block_size / 1024);

	printf("\n[1] Read before erase:\n");
	if ((ret = do_read(dev, TEST_OFFSET))) { return ret; }

	printf("\n[2] Erase block 0:\n");
	if ((ret = do_erase(dev, TEST_BLOCK))) { return ret; }

	printf("\n[3] Read after erase (expect 0xFF):\n");
	if ((ret = do_read(dev, TEST_OFFSET))) { return ret; }

	printf("\n[4] Write+verify pattern:\n");
	if ((ret = do_write(dev, TEST_OFFSET))) { return ret; }

	printf("\n[5] Final read:\n");
	if ((ret = do_read(dev, TEST_OFFSET))) { return ret; }

	printf("\n=== ALL PASSED ===\n");
	return 0;
}

int main(int argc, char *argv[]) {
	struct flash_dev *dev;
	uint32_t val;
	int opt;

	dev = flash_by_id(0);
	if (!dev) {
		fprintf(stderr, "spi_nor_test: no flash device found\n");
		return -ENODEV;
	}

	if (argc == 1) {
		return do_full_test(dev);
	}

	while ((opt = getopt(argc, argv, "r:e:w:")) != -1) {
		val = (uint32_t)strtoul(optarg, NULL, 0);
		switch (opt) {
		case 'r': return do_read(dev, val);
		case 'e': return do_erase(dev, val);
		case 'w': return do_write(dev, val);
		default:
			fprintf(stderr,
			    "Usage: spi_nor_test [-r|-e|-w <value>]\n");
			return -EINVAL;
		}
	}
	return 0;
}
