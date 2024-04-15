/**
 * @file
 * @brief
 *
 * @date 27.03.2012
 * @author Andrey Gazukin
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 */

#include <ctype.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include <drivers/block_dev.h>
#include <drivers/block_dev/ramdisk/ramdisk.h>
#include <embox/unit.h>
#include <mem/misc/pool.h>
#include <mem/page.h>
#include <mem/phymem.h>
#include <util/binalign.h>
#include <util/err.h>
#include <lib/libds/indexator.h>

#define MAX_RAMDISK_QUANTITY OPTION_GET(NUMBER, ramdisk_quantity)
#define RAMDISK_SIZE         OPTION_GET(NUMBER, size)
#define RAMDISK_BLOCK_SIZE   OPTION_GET(NUMBER, block_size)

POOL_DEF(ramdisk_pool, struct ramdisk, MAX_RAMDISK_QUANTITY);
INDEX_DEF(ramdisk_idx, 0, MAX_RAMDISK_QUANTITY);

int ramdisk_read_sectors(struct block_dev *bdev, char *buffer, size_t count,
    blkno_t blkno) {
	struct ramdisk *ramdisk;
	char *read_addr;

	ramdisk = block_dev_priv(bdev);
	read_addr = ramdisk->p_start_addr + (blkno * bdev->block_size);

	memcpy(buffer, read_addr, count);
	return count;
}

int ramdisk_write_sectors(struct block_dev *bdev, char *buffer, size_t count,
    blkno_t blkno) {
	struct ramdisk *ramdisk;
	char *write_addr;

	ramdisk = block_dev_priv(bdev);
	write_addr = ramdisk->p_start_addr + (blkno * bdev->block_size);

	memcpy(write_addr, buffer, count);
	return count;
}

int rmadisk_ioctl(struct block_dev *bdev, int cmd, void *args, size_t size) {
	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return bdev->size / bdev->block_size;

	case IOCTL_GETBLKSIZE:
		return bdev->block_size;
	}
	return -ENOSYS;
}

static const struct block_dev_ops ramdisk_pio_driver = {
    .bdo_ioctl = rmadisk_ioctl,
    .bdo_read = ramdisk_read_sectors,
    .bdo_write = ramdisk_write_sectors,
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
	return err2ptr(err);
}

/* TODO rewrite it to be device remove function */
int ramdisk_delete(const char *name) {
	struct ramdisk *ram;
	struct block_dev *bdev;

	assert(name);

	bdev = block_dev_find(basename((char *)name));

	if (!bdev) {
		return -ENOENT;
	}

	ram = block_dev_priv(bdev);

	if (!pool_belong(&ramdisk_pool, ram)) {
		return -EINVAL;
	}

	phymem_free(ram->p_start_addr, bdev->size / PAGE_SIZE());
	index_free(&ramdisk_idx, ram->idx);
	pool_free(&ramdisk_pool, ram);

	block_dev_free(bdev);

	return 0;
}
