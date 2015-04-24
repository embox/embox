/**
 * @file
 * @brief
 *
 * @date 27.03.2012
 * @author Andrey Gazukin
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

#include <embox/block_dev.h>

#include <drivers/ramdisk.h>

#define MAX_DEV_QUANTITY OPTION_GET(NUMBER,ramdisk_quantity)
#define RAMDISK_BLOCK_SIZE OPTION_GET(NUMBER,block_size)

POOL_DEF(ramdisk_pool,struct ramdisk,MAX_DEV_QUANTITY);
INDEX_DEF(ramdisk_idx,0,MAX_DEV_QUANTITY);

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

static int ramdisk_get_index(char *path) {
	char *dev_name;
	int idx;

	if(NULL == (dev_name = strstr(path, "ram"))) {
		return -1;
	}
	dev_name += sizeof("ram");

	if(!isdigit((int)dev_name[0])) {
		return -1;
	}

	sscanf(dev_name, "%d", &idx);

	return idx;
}

/* XXX not stores index if path have no index placeholder, like * or # */
struct ramdisk *ramdisk_create(char *path, size_t size) {
	struct ramdisk *ramdisk;
	int idx;
	int err;

	const size_t ramdisk_size = binalign_bound(size, RAMDISK_BLOCK_SIZE);
	const size_t page_n = (ramdisk_size + PAGE_SIZE() - 1) / PAGE_SIZE();

	if (NULL == (ramdisk = pool_alloc(&ramdisk_pool))) {
		err = ENOMEM;
		goto err_out;
	}

	ramdisk->blocks = ramdisk_size / RAMDISK_BLOCK_SIZE;
	ramdisk->block_size = RAMDISK_BLOCK_SIZE;

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

ramdisk_t *ramdisk_get_param(char *path) {
	struct path ramdisk_node;
	struct nas *nas;
	struct node_fi *node_fi;

	if (vfs_lookup(path, &ramdisk_node)) {
		return NULL;
	}

	if (NULL == ramdisk_node.node) {
		return NULL;
	}
	nas = ramdisk_node.node->nas;
	node_fi = nas->fi;
	return (ramdisk_t *) block_dev(node_fi->privdata)->privdata;
}

int ramdisk_delete(const char *name) {
	struct path ramdisk_node;
	ramdisk_t *ramdisk;
	struct nas *nas;
	struct node_fi *node_fi;
	int idx;

	if (vfs_lookup(name, &ramdisk_node)) {
		return -1;
	}

	if (NULL == ramdisk_node.node) {
		return -1;
	}

	nas = ramdisk_node.node->nas;
	node_fi = nas->fi;
	if (NULL != (ramdisk = (ramdisk_t *) block_dev(node_fi->privdata)->privdata)) {
		if (-1 != (idx = ramdisk_get_index((char *)name))) {
			index_free(&ramdisk_idx, idx);
		}
		pool_free(&ramdisk_pool, ramdisk);
		block_dev_destroy (node_fi->privdata);
		vfs_del_leaf(ramdisk_node.node);
	}
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

/*
static int flush(void *bdev) {
	return 0;
}
*/

EMBOX_BLOCK_DEV("ramdisk", &ramdisk_pio_driver, ram_init);
