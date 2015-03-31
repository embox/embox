/**
 * @file
 * @brief
 *
 * @date 22.08.2013
 * @author Andrey Gazukin
 */


#include <embox/block_dev.h>
#include <drivers/block_dev/flash/flash.h>
#include <drivers/block_dev/flash/flash_dev.h>
#include <mem/sysmalloc.h>

#include <util/err.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fs/vfs.h>

#define FLASH_ERASE_BLOCK_SIZE 4096

int flash_emu_init (void *arg) {

	return 0;
}

static size_t flash_emu_query (struct flash_dev *dev,
		void * data, size_t len) {

	return 0;
}

static int flash_emu_erase_block (struct flash_dev *dev, uint32_t block_base) {
	block_dev_t *bdev;
	int len;
	char * data;
	int rc;

	bdev = dev->privdata;
	if(NULL == bdev) {
		return -ENODEV;
	}
	len = bdev->driver->ioctl(bdev, IOCTL_GETBLKSIZE, NULL, 0);

	if(NULL == (data = sysmalloc(len))) {
		return -ENOMEM;
	}
	memset((void *) data, 0xFF, (size_t) len);

	rc = block_dev_write_buffered(bdev, (const char *) data,
			(size_t) len, block_base);
	sysfree(data);

	if(len == rc) {
		return 0;
	}

	return rc;
}

static int flash_emu_program (struct flash_dev *dev, uint32_t base,
		const void* data, size_t len) {
	block_dev_t *bdev;

	bdev = dev->privdata;
	if(NULL == bdev) {
		return -ENODEV;
	}
	return block_dev_write_buffered(bdev, (const char *) data,
			len, (uint32_t) base);
}

static int flash_emu_read (struct flash_dev *dev,
		uint32_t base, void* data, size_t len) {
	block_dev_t *bdev;

	bdev = dev->privdata;
	if(NULL == bdev) {
		return -ENODEV;
	}
	return block_dev_read_buffered(bdev, (char *) data,
			len, (uint32_t) base);
}

static struct flash_dev_drv flash_emu_drv = {
	flash_emu_init,
	flash_emu_query,
	flash_emu_erase_block,
	flash_emu_program,
	flash_emu_read
};

int flash_emu_dev_create (struct node *bdev_node, /*const*/ char *flash_node_path) {
	struct flash_dev *flash;
	struct block_dev * bdev;
	size_t block_size, factor;

	if(NULL == (bdev = (struct block_dev *)
				bdev_node->nas->fi->privdata)) {
		return -ENOTBLK;
	}

	flash = flash_create(flash_node_path, bdev->size);
	if(err(flash)) {
		return err(flash);
	}

	flash->privdata = bdev;
	/* XXX */
	flash->block_info.block_size = FLASH_ERASE_BLOCK_SIZE;
	block_size = bdev->driver->ioctl(bdev, IOCTL_GETBLKSIZE, NULL, 0);
	factor = FLASH_ERASE_BLOCK_SIZE / block_size;
	if(0 == factor) {
		factor++;
	}
	flash->block_info.blocks = bdev->driver->ioctl(bdev, IOCTL_GETDEVSIZE, NULL, 0);
	flash->block_info.blocks /= factor;

	flash->start = 0;
	flash->end = bdev->size;

	flash->drv = &flash_emu_drv;

	return 0;
}

int flash_emu_dev_init (void *arg) {
	struct node *bdev_node;
	char **argv;

	argv = (char **) arg;

	if(NULL == arg) {
		return 0;
	}

	if(NULL == (bdev_node = vfs_subtree_lookup(NULL, argv[1]))) {
		return -ENOENT;
	}

	return flash_emu_dev_create(bdev_node, argv[2]);
}

EMBOX_FLASH_DEV("flash_emul", &flash_emu_drv, flash_emu_dev_init);

