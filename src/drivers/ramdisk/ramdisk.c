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

static int ioctl(void *dev);
static int flush(void *dev);
static size_t read_sectors(void *dev, char *buffer,
		uint32_t sector, uint32_t count);
static size_t write_sectors(void *dev, char *buffer,
		uint32_t sector, uint32_t count);

static block_dev_operations_t block_dev_op = {
		.blk_read = read_sectors,
		.blk_write = write_sectors,
		.ioctl = ioctl,
		.flush = flush
};

typedef struct dev_ramdisk_head {
	dev_ramdisk_t param;
} dev_ramdisk_head_t;

POOL_DEF ( ramdisk_pool, struct dev_ramdisk_head, OPTION_GET(NUMBER,ramdisk_quantity ));

EMBOX_UNIT_INIT(unit_init);

int ramdisk_create(void *mkfs_params) {
	node_t *ramdisk_node;
	mkfs_params_t *p_mkfs_params;
	dev_ramdisk_t *ram_disk;
	block_dev_module_t *block_dev;

	p_mkfs_params = (mkfs_params_t *)mkfs_params;

	if (NULL == (ramdisk_node = vfs_add_path(p_mkfs_params->path, NULL))) {
		return -EBUSY;/*file already exist*/
	}

	ram_disk = pool_alloc(&ramdisk_pool);
	ramdisk_node->dev_attr = (void *) ram_disk;
	ram_disk->dev_node = ramdisk_node;

	if(NULL == (block_dev = block_dev_find("ramdisk"))) {
		return -ENODEV;
	}
	ramdisk_node->dev_type = (void *) block_dev->dev_ops;

	if(NULL == (ram_disk->p_start_addr =
			page_alloc(p_mkfs_params->blocks))) {
		return -ENOMEM;
	}

	strcpy ((void *)&ram_disk->path, (const void *)p_mkfs_params->path);
	ram_disk->size = p_mkfs_params->blocks * PAGE_SIZE();
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
	return (dev_ramdisk_t *) ramdisk_node->dev_attr;
}

int ramdisk_delete(const char *name) {
	node_t *ramdisk_node;
	dev_ramdisk_t *ram_disk;

	if (NULL == (ramdisk_node = vfs_find_node(name, NULL))) {
		return -1;
	}
	ram_disk = ramdisk_node->dev_attr;

	pool_free(&ramdisk_pool, ram_disk);
	vfs_del_leaf(ramdisk_node);
	return 0;
}

static int unit_init(void) {
	//init_ramdisk_info_pool();
	return 0;
}

static size_t read_sectors(void *dev, char *buffer,
		uint32_t sector, uint32_t count) {
	dev_ramdisk_t *ram_disk;
	char *read_addr;
	uint32_t size;

	ram_disk = (dev_ramdisk_t *) dev;

	size = ram_disk->sector_size * count;
	read_addr = ram_disk->p_start_addr + (sector * ram_disk->sector_size);

	memcpy(buffer, read_addr, size);
	return 0;
}


static size_t write_sectors(void *dev, char *buffer,
		uint32_t sector, uint32_t count) {
	dev_ramdisk_t *ram_disk;
	char *write_addr;
	uint32_t size;

	ram_disk = (dev_ramdisk_t *) dev;

	size = ram_disk->sector_size * count;
	write_addr = ram_disk->p_start_addr + (sector * ram_disk->sector_size);

	memcpy(write_addr, buffer, size);
	return 0;
}

static int ioctl(void *dev) {

	return 0;
}
static int flush(void *dev) {

	return 0;
}

EMBOX_BLOCK_DEV("ramdisk", &block_dev_op);
