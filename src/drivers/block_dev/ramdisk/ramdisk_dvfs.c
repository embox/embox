/**
 * @file
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-01
 */

#include <errno.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <ctype.h>

#include <util/err.h>
#include <embox/unit.h>
#include <fs/vfs.h>

#include <mem/page.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h>

#include <util/indexator.h>
#include <util/binalign.h>

#include <drivers/block_dev.h>

#include <drivers/block_dev/ramdisk/ramdisk.h>

#define MAX_DEV_QUANTITY OPTION_GET(NUMBER,ramdisk_quantity)
#define RAMDISK_BLOCK_SIZE OPTION_GET(NUMBER,block_size)

POOL_DEF(ramdisk_pool,struct ramdisk,MAX_DEV_QUANTITY);
INDEX_DEF(ramdisk_idx, 0, MAX_DEV_QUANTITY);

static int ram_init(void *arg);
static int read_sectors(struct block_dev *bdev, char *buffer, size_t count, blkno_t blkno);
static int write_sectors(struct block_dev *bdev, char *buffer, size_t count, blkno_t blkno);
static int ram_ioctl(struct block_dev *bdev, int cmd, void *args, size_t size);

block_dev_driver_t ramdisk_pio_driver = {
	"ramdisk_drv",
	ram_ioctl,
	read_sectors,
	write_sectors
};

/* XXX not stores index if path have no index placeholder, like * or # */
struct ramdisk *ramdisk_create(char *path, size_t size) {
	char buf[256];
	strcpy(buf, "hdr#");
	if (0 > block_dev_named(buf, &ramdisk_idx))
		return NULL;

	block_dev_create(buf, &ramdisk_pio_driver, NULL);
	return NULL;
}

ramdisk_t *ramdisk_get_param(char *path) {
	return 0;
}

int ramdisk_delete(const char *name) {
	/* TODO Delete corresponding idx */
	return 0;
}


static int ram_init(void *arg) {
	return 0;
}

static int read_sectors(block_dev_t *bdev,
		char *buffer, size_t count, blkno_t blkno) {
	ramdisk_t *ramdisk;
	char *read_addr;

	ramdisk = (ramdisk_t *) bdev->privdata;
	read_addr = ramdisk->p_start_addr + (blkno * ramdisk->block_size);

	memcpy(buffer, read_addr, count);
	return count;
}


static int write_sectors(block_dev_t *bdev,
		char *buffer, size_t count, blkno_t blkno) {
	ramdisk_t *ramdisk;
	char *write_addr;

	ramdisk = (ramdisk_t *) bdev->privdata;
	write_addr = ramdisk->p_start_addr + (blkno * ramdisk->block_size);

	memcpy(write_addr, buffer, count);
	return count;
}

static int ram_ioctl(block_dev_t *bdev, int cmd, void *args, size_t size) {
	ramdisk_t *ramd = (ramdisk_t *) bdev->privdata;

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return ramd->blocks;

	case IOCTL_GETBLKSIZE:
		return ramd->block_size;
	}
	return -ENOSYS;
}

EMBOX_BLOCK_DEV("ramdisk", &ramdisk_pio_driver, ram_init);
