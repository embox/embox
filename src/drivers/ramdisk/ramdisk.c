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
#define RAMDISK_BLOCK_SIZE  PAGE_SIZE()
typedef struct dev_ramdisk_head {
	ramdisk_t param;
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

int ramdisk_create(void *params) {
	ramdisk_create_params_t *new_ramdisk;
	ramdisk_t *ram_disk;

	if(NULL == (new_ramdisk = (ramdisk_create_params_t *)params)) {
		return -ENOENT;
	}

	if(NULL == (ram_disk = pool_alloc(&ramdisk_pool))) {
		return -ENOMEM;
	}
	ram_disk->idx = (dev_t) index_alloc(&ramdisk_idx, INDEX_ALLOC_MIN);
	if(0 > (ram_disk->dev_id = block_dev_create(new_ramdisk->path, &ramdisk_pio_driver,
			ram_disk, &ram_disk->idx))) {
		index_free(&ramdisk_idx, ram_disk->idx);
		pool_free(&ramdisk_pool, ram_disk);
		return -EIO;
	}

	ram_disk->dev_node = block_dev(ram_disk->dev_id)->dev_node;

	ram_disk->blocks = new_ramdisk->size / RAMDISK_BLOCK_SIZE;
	if(new_ramdisk->size % RAMDISK_BLOCK_SIZE) {
		ram_disk->blocks++;
	}
	if(NULL == (ram_disk->p_start_addr =
			page_alloc(__phymem_allocator, ram_disk->blocks))) {
		block_dev_destroy(ram_disk->dev_id);
		index_free(&ramdisk_idx, ram_disk->idx);
		pool_free(&ramdisk_pool, ram_disk);
		return -ENOMEM;
	}

	strcpy ((void *)&ram_disk->path, (const void *)new_ramdisk->path);
	ram_disk->size = ram_disk->blocks * RAMDISK_BLOCK_SIZE;
	block_dev(ram_disk->dev_id)->size = ram_disk->size;
	ram_disk->block_size = PAGE_SIZE();

	strcpy ((void *)ram_disk->fs_name,
				(const void *)new_ramdisk->fs_name);
	ram_disk->fs_type = new_ramdisk->fs_type;

	return 0;
}

ramdisk_t *ramdisk_get_param(char *path) {
	node_t *ramdisk_node;

	if (NULL == (ramdisk_node = vfs_find_node(path, NULL))) {
		return NULL;
	}
	return (ramdisk_t *) block_dev(ramdisk_node->dev_id)->privdata;
}

int ramdisk_delete(const char *name) {
	node_t *ramdisk_node;
	ramdisk_t *ram_disk;

	if (NULL == (ramdisk_node = vfs_find_node(name, NULL))) {
		return -1;
	}
	if(NULL != (ram_disk = (ramdisk_t *) block_dev(ramdisk_node->dev_id)->privdata)) {
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
	ramdisk_t *ram_disk;
	char *read_addr;

	ram_disk = (ramdisk_t *) dev->privdata;
	read_addr = ram_disk->p_start_addr + (blkno * ram_disk->block_size);

	memcpy(buffer, read_addr, count);
	return count;
}


static int write_sectors(block_dev_t *dev,
		char *buffer, size_t count, blkno_t blkno) {
	ramdisk_t *ram_disk;
	char *write_addr;

	ram_disk = (ramdisk_t *) dev->privdata;
	write_addr = ram_disk->p_start_addr + (blkno * ram_disk->block_size);

	memcpy(write_addr, buffer, count);
	return count;
}

static int ram_ioctl(block_dev_t *dev, int cmd, void *args, size_t size) {
	ramdisk_t *ramd = (ramdisk_t *) dev->privdata;

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return ramd->blocks;

	case IOCTL_GETBLKSIZE:
		return ramd->block_size;
	}
	return -ENOSYS;
}

/*
static int flush(void *dev) {
	return 0;
}
*/

EMBOX_BLOCK_DEV("ramdisk", &ramdisk_pio_driver, ram_init);
