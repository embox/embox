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
#include <drivers/device.h>
#include <drivers/block_dev/ramdisk/ramdisk.h>

#define MAX_DEV_QUANTITY OPTION_GET(NUMBER,ramdisk_quantity)
#define RAMDISK_BLOCK_SIZE OPTION_GET(NUMBER,block_size)

POOL_DEF(ramdisk_pool,struct ramdisk,MAX_DEV_QUANTITY);
INDEX_DEF(ramdisk_idx, 0, MAX_DEV_QUANTITY);

static int read_sectors(struct block_dev *bdev, char *buffer, size_t count, blkno_t blkno);
static int write_sectors(struct block_dev *bdev, char *buffer, size_t count, blkno_t blkno);

struct block_dev_driver ramdisk_pio_driver = {
	.name  = "ramdisk_drv",
	.read  = read_sectors,
	.write = write_sectors
};

struct ramdisk *ramdisk_create(char *path, size_t size) {
	char buf[256];
	struct block_dev *bdev;
	struct ramdisk *ram;
	const size_t ramdisk_size = binalign_bound(size, RAMDISK_BLOCK_SIZE);
	const size_t page_n = (ramdisk_size + PAGE_SIZE() - 1) / PAGE_SIZE();

	if (NULL == (ram = pool_alloc(&ramdisk_pool)))
		goto err_out;

	ram->blocks = ramdisk_size / RAMDISK_BLOCK_SIZE;
	ram->p_start_addr = phymem_alloc(page_n);
	if (NULL == (ram->p_start_addr))
		goto err_free_ramdisk;

	strcpy(buf, dvfs_last_link(path));
	if (buf[strlen(buf) - 1] == '/')
		goto err_free_mem;

	if (0 > (ram->idx = block_dev_named(buf, &ramdisk_idx)))
		goto err_free_mem;

	bdev = block_dev_create(buf, &ramdisk_pio_driver, NULL);
	if (!bdev)
		goto err_free_mem;

	bdev->privdata = ram;
	bdev->block_size = RAMDISK_BLOCK_SIZE;
	bdev->size = ramdisk_size;

	return ram;
err_free_mem:
	phymem_free(ram->p_start_addr, page_n);
err_free_ramdisk:
	pool_free(&ramdisk_pool, ram);
err_out:
	return NULL;
}

ramdisk_t *ramdisk_get_param(char *path) {
	return 0;
}

/* TODO rewrite it to be device remove function */
int ramdisk_delete(const char *name) {
	struct ramdisk *ram;
	struct block_dev *bdev;
	size_t ramsize;

	assert(name);

	bdev = block_dev_find(name);

	if (!bdev)
		return -ENOENT;

	ram = bdev->privdata;

	if (!pool_belong(&ramdisk_pool, ram))
		return -EINVAL;

	ramsize = ram->blocks * RAMDISK_BLOCK_SIZE + PAGE_SIZE() - 1;

	phymem_free(ram->p_start_addr, ramsize / PAGE_SIZE());
	index_free(&ramdisk_idx, ram->idx);
	pool_free(&ramdisk_pool, ram);

	block_dev_free(bdev);

	return 0;
}

static int read_sectors(struct block_dev *bdev,
		char *buffer, size_t count, blkno_t blkno) {
	ramdisk_t *ramdisk;
	char *read_addr;

	ramdisk = (ramdisk_t *) bdev->privdata;
	read_addr = ramdisk->p_start_addr + (blkno * bdev->block_size);

	memcpy(buffer, read_addr, count);
	return count;
}


static int write_sectors(struct block_dev *bdev,
		char *buffer, size_t count, blkno_t blkno) {
	ramdisk_t *ramdisk;
	char *write_addr;

	ramdisk = (ramdisk_t *) bdev->privdata;
	write_addr = ramdisk->p_start_addr + (blkno * bdev->block_size);

	memcpy(write_addr, buffer, count);
	return count;
}
