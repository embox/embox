/**
 * @file
 * @brief
 *
 * @date 27.03.2012
 * @author Andrey Gazukin
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 */

#include <errno.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <ctype.h>
#include <libgen.h>

#include <util/err.h>
#include <embox/unit.h>

#include <mem/page.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h>

#include <util/indexator.h>
#include <util/binalign.h>

#include <drivers/block_dev.h>

#include <drivers/block_dev/ramdisk/ramdisk.h>

#define MAX_RAMDISK_QUANTITY OPTION_GET(NUMBER,ramdisk_quantity)
#define RAMDISK_SIZE OPTION_GET(NUMBER,size)
#define RAMDISK_BLOCK_SIZE OPTION_GET(NUMBER,block_size)

POOL_DEF(ramdisk_pool,struct ramdisk,MAX_RAMDISK_QUANTITY);
INDEX_DEF(ramdisk_idx, 0, MAX_RAMDISK_QUANTITY);

static int read_sectors(struct block_dev *bdev,
		char *buffer, size_t count, blkno_t blkno) {
	struct ramdisk *ramdisk;
	char *read_addr;

	ramdisk = bdev->privdata;
	read_addr = ramdisk->p_start_addr + (blkno * bdev->block_size);

	memcpy(buffer, read_addr, count);
	return count;
}


static int write_sectors(struct block_dev *bdev,
		char *buffer, size_t count, blkno_t blkno) {
	struct ramdisk *ramdisk;
	char *write_addr;

	ramdisk = bdev->privdata;
	write_addr = ramdisk->p_start_addr + (blkno * bdev->block_size);

	memcpy(write_addr, buffer, count);
	return count;
}

static int ram_ioctl(struct block_dev *bdev, int cmd, void *args, size_t size) {
	struct ramdisk *ramd = bdev->privdata;

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return ramd->blocks;

	case IOCTL_GETBLKSIZE:
		return bdev->block_size;
	}
	return -ENOSYS;
}

static const struct block_dev_driver ramdisk_pio_driver = {
	.name  = "ramdisk_drv",
	.ioctl = ram_ioctl,
	.read = read_sectors,
	.write = write_sectors
};

/* XXX not stores index if path have no index placeholder, like * or # */
struct ramdisk *ramdisk_create(char *path, size_t size) {
	struct ramdisk *ramdisk;
	int idx;
	int err;

	const size_t ramdisk_size = binalign_bound(size, RAMDISK_SIZE);
	const size_t page_n = (ramdisk_size + PAGE_SIZE() - 1) / PAGE_SIZE();

	if (NULL == (ramdisk = pool_alloc(&ramdisk_pool))) {
		err = ENOMEM;
		goto err_out;
	}

	ramdisk->blocks = ramdisk_size / PAGE_SIZE();

	ramdisk->p_start_addr = phymem_alloc(page_n);
	if (NULL == (ramdisk->p_start_addr)) {
		err = ENOMEM;
		goto err_free_ramdisk;
	}

	if (0 > (idx = block_dev_named(path, &ramdisk_idx))) {
		err = -idx;
		goto err_free_mem;
	}

	ramdisk->bdev = block_dev_create(path, &ramdisk_pio_driver, ramdisk);
	if (NULL == ramdisk->bdev) {
		err = EIO;
		goto err_free_bdev_idx;
	}

	ramdisk->bdev->size = ramdisk_size;
	ramdisk->bdev->block_size = RAMDISK_BLOCK_SIZE;
	return ramdisk;

err_free_bdev_idx:
	index_free(&ramdisk_idx, idx);
err_free_mem:
	phymem_free(ramdisk->p_start_addr, page_n);
err_free_ramdisk:
	pool_free(&ramdisk_pool, ramdisk);
err_out:
	return err_ptr(err);
}

/* TODO rewrite it to be device remove function */
int ramdisk_delete(const char *name) {
	struct ramdisk *ram;
	struct block_dev *bdev;
	size_t ramsize;

	assert(name);

	bdev = block_dev_find(basename((char *)name));

	if (!bdev) {
		return -ENOENT;
	}

	ram = bdev->privdata;

	if (!pool_belong(&ramdisk_pool, ram)) {
		return -EINVAL;
	}

	ramsize = ram->blocks * RAMDISK_BLOCK_SIZE + PAGE_SIZE() - 1;

	phymem_free(ram->p_start_addr, ramsize / PAGE_SIZE());
	index_free(&ramdisk_idx, ram->idx);
	pool_free(&ramdisk_pool, ram);

	block_dev_free(bdev);

	return 0;
}
