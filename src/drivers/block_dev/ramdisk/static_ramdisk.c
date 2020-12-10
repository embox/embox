/**
 * @file
 *
 * @date Dec 8, 2020
 * @author Anton Bondarev
 */
#include <stdint.h>
#include <errno.h>

#include <drivers/block_dev.h>
#include "ramdisk.h"

#include <framework/mod/options.h>

#define STATIC_RAMDISK_NAME       OPTION_STRING_GET(ramdisk_name)
#define STATIC_RAMDISK_SIZE       OPTION_GET(NUMBER,size)
#define STATIC_RAMDISK_BLOCK_SIZE OPTION_GET(NUMBER,block_size)

#define STATIC_RAMDISK_PATH    "/dev/"STATIC_RAMDISK_NAME

extern int ramdisk_read_sectors(struct block_dev *bdev,
		char *buffer, size_t count, blkno_t blkno);
extern int ramdisk_write_sectors(struct block_dev *bdev,
		char *buffer, size_t count, blkno_t blkno);
extern int rmadisk_ioctl(struct block_dev *bdev, int cmd, void *args, size_t size);

static struct ramdisk static_ramdisk;

static uint8_t static_ramdisk_buffer[STATIC_RAMDISK_SIZE] __attribute__ ((aligned(STATIC_RAMDISK_BLOCK_SIZE)));

static const struct block_dev_ops static_ramdisk_pio_driver;

static int static_ramdisk_init (void *args) {
	struct block_dev *bdev;

	static_ramdisk.p_start_addr = (char *)static_ramdisk_buffer;
	bdev = block_dev_create(STATIC_RAMDISK_PATH, &static_ramdisk_pio_driver, &static_ramdisk);
	if (NULL == bdev) {
		return -EIO;
	}

	bdev->size = STATIC_RAMDISK_SIZE;
	bdev->block_size = STATIC_RAMDISK_BLOCK_SIZE;
	static_ramdisk.bdev = bdev;

	return 0;
}

static const struct block_dev_ops static_ramdisk_pio_driver = {
		.name  = "ramdisk_drv",
		.ioctl = rmadisk_ioctl,
		.read = ramdisk_read_sectors,
		.write = ramdisk_write_sectors,
		.probe = static_ramdisk_init
};

BLOCK_DEV_DRIVER_DEF(STATIC_RAMDISK_NAME, &static_ramdisk_pio_driver);
