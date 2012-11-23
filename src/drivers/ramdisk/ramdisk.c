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

//EMBOX_UNIT_INIT(unit_init);

int ramdisk_create(void *params) {
	ramdisk_create_params_t *new_ramdisk;
	ramdisk_t *ramdisk;

	if(NULL == (new_ramdisk = (ramdisk_create_params_t *)params)) {
		return -ENOENT;
	}

	if(NULL == (ramdisk = pool_alloc(&ramdisk_pool))) {
		return -ENOMEM;
	}
	if(0 > (ramdisk->idx = block_dev_named(new_ramdisk->path, &ramdisk_idx))) {
		return -ENOENT;
	}

	if(NULL == (ramdisk->bdev = block_dev_create(new_ramdisk->path,
			&ramdisk_pio_driver, ramdisk))) {
		index_free(&ramdisk_idx, ramdisk->idx);
		pool_free(&ramdisk_pool, ramdisk);
		return -EIO;
	}

	ramdisk->dev_node = block_dev(ramdisk->bdev)->dev_node;

	ramdisk->blocks = new_ramdisk->size / RAMDISK_BLOCK_SIZE;
	if(new_ramdisk->size % RAMDISK_BLOCK_SIZE) {
		ramdisk->blocks++;
	}
	if(NULL == (ramdisk->p_start_addr =
			page_alloc(__phymem_allocator, ramdisk->blocks))) {
		block_dev_destroy(ramdisk->bdev);
		index_free(&ramdisk_idx, ramdisk->idx);
		pool_free(&ramdisk_pool, ramdisk);
		return -ENOMEM;
	}

	strncpy ((void *)&ramdisk->path,
			 (const void *)new_ramdisk->path, MAX_LENGTH_PATH_NAME);
	ramdisk->size = ramdisk->blocks * RAMDISK_BLOCK_SIZE;
	block_dev(ramdisk->bdev)->size = ramdisk->size;
	ramdisk->block_size = PAGE_SIZE();

	strncpy ((void *)ramdisk->fs_name,
			 (const void *)new_ramdisk->fs_name, MAX_LENGTH_FILE_NAME);
	ramdisk->fs_type = new_ramdisk->fs_type;

	return 0;
}

ramdisk_t *ramdisk_get_param(char *path) {
	node_t *ramdisk_node;

	if (NULL == (ramdisk_node = vfs_find_node(path, NULL))) {
		return NULL;
	}
	return (ramdisk_t *) block_dev(ramdisk_node->node_info)->privdata;
}

int ramdisk_delete(const char *name) {
	node_t *ramdisk_node;
	ramdisk_t *ramdisk;

	if (NULL == (ramdisk_node = vfs_find_node(name, NULL))) {
		return -1;
	}
	if(NULL != (ramdisk = (ramdisk_t *) block_dev(ramdisk_node->node_info)->privdata)) {
		index_free(&ramdisk_idx, ramdisk->idx);
		pool_free(&ramdisk_pool, ramdisk);
		block_dev_destroy (ramdisk_node->node_info);
		vfs_del_leaf(ramdisk_node);
	}
	return 0;
}

//static int unit_init(void) {
//	return 0;
//}

static int ram_init(void *arg) {
	return 0;
}

static int read_sectors(block_dev_t *dev,
		char *buffer, size_t count, blkno_t blkno) {
	ramdisk_t *ramdisk;
	char *read_addr;

	ramdisk = (ramdisk_t *) dev->privdata;
	read_addr = ramdisk->p_start_addr + (blkno * ramdisk->block_size);

	memcpy(buffer, read_addr, count);
	return count;
}


static int write_sectors(block_dev_t *dev,
		char *buffer, size_t count, blkno_t blkno) {
	ramdisk_t *ramdisk;
	char *write_addr;

	ramdisk = (ramdisk_t *) dev->privdata;
	write_addr = ramdisk->p_start_addr + (blkno * ramdisk->block_size);

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
