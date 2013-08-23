/**
 * @file
 * @brief
 *
 * @date 22.08.2013
 * @author Andrey Gazukin
 */


#include <embox/block_dev.h>

#include <drivers/flash/flash.h>
#include <drivers/flash/flash_dev.h>

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


static int flash_emu_init (void *arg) {

	if(NULL == arg) {
		return 0;
	}

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

	bdev = dev->privdata;
	if(NULL == bdev) {
		return -ENODEV;
	}
	len = bdev->driver->ioctl(bdev, IOCTL_GETBLKSIZE, NULL, 0);

	if(NULL == (data = malloc(len))) {
		return -ENOMEM;
	}
	memset((void *) data, 0, (size_t) len);

	len = block_dev_write_buffered(bdev, (const char *) data,
									(size_t) len, block_base);
	free(data);

	return len;
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

EMBOX_FLASH_DEV("flash_emul", &flash_emu_drv, flash_emu_init);

