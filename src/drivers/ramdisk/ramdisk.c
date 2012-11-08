/**
 * @file
 * @brief
 *
 * @date 27.03.2012
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <string.h>
#include <cmd/mkfs.h>
#include <embox/unit.h>
#include <fs/vfs.h>
#include <drivers/ramdisk.h>
#include <mem/page.h>
#include <mem/misc/pool.h>
#include <embox/block_dev.h>
#include <mem/phymem.h>
#include <util/indexator.h>

#define MAX_DEV_QUANTITY OPTION_GET(NUMBER,ramdisk_quantity)

typedef struct dev_ramdisk_head {
	dev_ramdisk_t param;
} dev_ramdisk_head_t;

POOL_DEF(ramdisk_pool,struct dev_ramdisk_head,MAX_DEV_QUANTITY);
INDEX_DEF(ramdisk_idx,0,MAX_DEV_QUANTITY);

static int ram_init(void *arg);
static int read_sectors(block_dev_t *dev, char *buffer, size_t count, blkno_t blkno);
static int write_sectors(block_dev_t *dev, char *buffer, size_t count, blkno_t blkno);
static int ram_ioctl(block_dev_t *dev, int cmd, void *args, size_t size);

block_dev_driver_t ramdisk_pio_driver = {
  "ramdisk_drv",
  ram_ioctl,
  read_sectors,
  write_sectors
};

EMBOX_UNIT_INIT(unit_init);

int ramdisk_create(void *mkfs_params) {
	mkfs_params_t *p_mkfs_params;
	dev_ramdisk_t *ram_disk;

	p_mkfs_params = (mkfs_params_t *)mkfs_params;

	if(NULL == (ram_disk = pool_alloc(&ramdisk_pool))) {
		return -ENOMEM;
	}
	ram_disk->idx = (dev_t) index_alloc(&ramdisk_idx, INDEX_ALLOC_MIN);
	if(0 > (ram_disk->dev_id = block_dev_make(p_mkfs_params->path, &ramdisk_pio_driver,
			ram_disk, &ram_disk->idx))) {
		index_free(&ramdisk_idx, ram_disk->idx);
		return -EIO;
	}

	ram_disk->dev_node = block_dev(ram_disk->dev_id)->dev_node;

	if(NULL == (ram_disk->p_start_addr =
			page_alloc(__phymem_allocator, p_mkfs_params->blocks))) {
		return -ENOMEM;
	}

	strcpy ((void *)&ram_disk->path, (const void *)p_mkfs_params->path);
	ram_disk->size = p_mkfs_params->blocks * PAGE_SIZE();
	block_dev(ram_disk->dev_id)->size = ram_disk->size;
	ram_disk->blocks = p_mkfs_params->blocks;
	ram_disk->sector_size = 512;

	strcpy ((void *)&ram_disk->fs_name,
			(const void *)p_mkfs_params->fs_name);

	return 0;
}

dev_ramdisk_t *ramdisk_get_param(char *name) {
	node_t *ramdisk_node;

	if (NULL == (ramdisk_node = vfs_find_node(name, NULL))) {
		return NULL;
	}
	return (dev_ramdisk_t *) block_dev(ramdisk_node->dev_id)->privdata;
}

int ramdisk_delete(const char *name) {
	node_t *ramdisk_node;
	dev_ramdisk_t *ram_disk;

	if (NULL == (ramdisk_node = vfs_find_node(name, NULL))) {
		return -1;
	}
	if(NULL != (ram_disk = (dev_ramdisk_t *) block_dev(ramdisk_node->dev_id)->privdata)) {
		index_free(&ramdisk_idx, ram_disk->idx);
		pool_free(&ramdisk_pool, ram_disk);
		block_dev_destroy (ramdisk_node->dev_id);
		vfs_del_leaf(ramdisk_node);
	}
	return 0;
}

static int unit_init(void) {
	return 0;
}

static int ram_init(void *arg) {
	return 0;
}

static int read_sectors(block_dev_t *dev,
		char *buffer, size_t count, blkno_t blkno) {
	dev_ramdisk_t *ram_disk;
	char *read_addr;

	ram_disk = (dev_ramdisk_t *) dev->privdata;
	read_addr = ram_disk->p_start_addr + (blkno * ram_disk->sector_size);

	memcpy(buffer, read_addr, count);
	return count;
}


static int write_sectors(block_dev_t *dev,
		char *buffer, size_t count, blkno_t blkno) {
	dev_ramdisk_t *ram_disk;
	char *write_addr;

	ram_disk = (dev_ramdisk_t *) dev->privdata;
	write_addr = ram_disk->p_start_addr + (blkno * ram_disk->sector_size);

	memcpy(write_addr, buffer, count);
	return count;
}

static int ram_ioctl(block_dev_t *dev, int cmd, void *args, size_t size) {
	dev_ramdisk_t *ramd = (dev_ramdisk_t *) dev->privdata;

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return ramd->blocks;

	case IOCTL_GETBLKSIZE:
		return ramd->sector_size;
	}
	return -ENOSYS;
}

/*
static int flush(void *dev) {
	return 0;
}
*/

EMBOX_BLOCK_DEV("ramdisk", &ramdisk_pio_driver, ram_init);
