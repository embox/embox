/**
 * @brief Interface for flash subsystem. Device will appear in devfs
 *
 * @date 21.08.2013
 * @author Andrey Gazukin
 * @author Denis Deryugin
 */

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include <drivers/block_dev.h>
#include <drivers/flash/flash.h>
#include <util/err.h>

/* Interface for block device in devfs */
static int flashbdev_ioctl(struct block_dev *bdev, int cmd, void *buf,
    size_t size);
static int flashbdev_read(struct block_dev *bdev, char *buffer, size_t count,
    blkno_t blkno);
static int flashbdev_write(struct block_dev *bdev, char *buffer, size_t count,
    blkno_t blkno);

static const struct block_dev_ops flashbdev_pio_driver = {
    .bdo_ioctl = flashbdev_ioctl,
    .bdo_read = flashbdev_read,
    .bdo_write = flashbdev_write,
};

struct flash_dev *flash_create(const char *name, size_t size) {
	struct flash_dev *flash;
	char dev_name[FILENAME_MAX];

	assert(name);

	if (NULL == (flash = flash_alloc())) {
		return err_ptr(ENOMEM);
	}

	memset(dev_name, 0, sizeof(dev_name));
	snprintf(dev_name, sizeof(dev_name), "%s%d", name, flash->idx);

	flash->bdev = block_dev_create(dev_name, &flashbdev_pio_driver, flash);
	if (NULL == flash->bdev) {
		flash_free(flash);
		return err_ptr(EIO);
	}

	flash->bdev->size = size;

	flash->fld_word_size = 4;
	flash->fld_aligned_word = NULL;

	return flash;
}

int flash_delete(struct flash_dev *dev) {
	block_dev_destroy(dev->bdev);
	flash_free(dev);
	return 0;
}

/* Calls for access via devfs */
static int flashbdev_read(struct block_dev *bdev, char *buffer, size_t count,
    blkno_t blkno) {
	struct flash_dev *flash;
	uint32_t offset;

	assert(bdev);

	flash = block_dev_priv(bdev);
	assert(flash);

	assert(flash->num_block_infos == 1); /* NIY for num_block_infos > 1 */

	offset = blkno * flash->block_info[0].block_size;

	return flash_read(flash, offset, buffer, count);
}

static int flashbdev_write(struct block_dev *bdev, char *buffer, size_t count,
    blkno_t blkno) {
	struct flash_dev *flash;
	uint32_t offset;

	assert(bdev);

	flash = block_dev_priv(bdev);
	assert(flash);
	assert(flash->num_block_infos == 1); /* NIY for num_block_infos > 1 */

	offset = blkno * flash->block_info[0].block_size;

	return flash_write(flash, offset, buffer, count);
}

static int flashbdev_erase(struct flash_dev *dev, uint32_t flash_base,
    size_t len, uint32_t *err_address) {
	uint32_t block, end_addr;
	size_t erase_count;
	int stat = 0;
	size_t block_size;

	assert(dev);
	assert(dev->drv);
	assert(dev->num_block_infos == 1); /* NIY for num_block_infos > 1 */

	if ((!dev->drv) || (!dev->drv->flash_erase_block)) {
		return -EINVAL;
	}

	/* Check whether or not we are going past the end of this device, on
	 * to the next one. If so the next device will be handled by a
	 * recursive call later on.
	 */
	if (len > (dev->size + 1 - flash_base)) {
		end_addr = dev->size;
	}
	else {
		end_addr = flash_base + len - 1;
	}
	/* erase can only happen on a block boundary, so adjust for this */
	block = flash_base;
	erase_count = (end_addr + 1) - block;
	block_size = dev->block_info[0].block_size;

	while (erase_count > 0) {
		if (erase_count < block_size) {
			erase_count = block_size;
		}

		stat = dev->drv->flash_erase_block(dev, block);

		if (0 != stat) {
			if (err_address) {
				*err_address = block;
			}
			break;
		}
		block += block_size;
		erase_count -= block_size;
	}
	return stat;
}

static int decode_flash_cmd(int cmd) {
	switch (cmd) {
	case IOCTL_GETBLKSIZE:
		return FLASH_IOCTL_BLOCKSIZE;
	case IOCTL_GETDEVSIZE:
		return FLASH_IOCTL_DEVSIZE;
	default:
		return cmd;
	}
}

static int flashbdev_ioctl(struct block_dev *bdev, int cmd, void *buf,
    size_t size) {
	struct flash_dev *dev;
	struct flash_ioctl_erase *e;
	uint32_t startpos, endpos;
	struct flash_ioctl_devsize *ds;
	struct flash_ioctl_devaddr *da;
	struct flash_ioctl_blocksize *bs;

	assert(bdev);

	dev = block_dev_priv(bdev);
	assert(dev);

	cmd = decode_flash_cmd(cmd);

	switch (cmd) {
	case FLASH_IOCTL_ERASE:
		e = (struct flash_ioctl_erase *)buf;
		startpos = e->offset;

		/* Unlike some other cases we _do_ do bounds checking on this all the time, because
		* the consequences of erasing the wrong bit of flash are so nasty.
		*/
		endpos = startpos + e->len - 1;

		if (endpos > dev->size) {
			return -EINVAL;
		}

		e->flasherr = flashbdev_erase(dev, startpos, e->len, &e->err_address);
		return ENOERR;

	case FLASH_IOCTL_DEVSIZE:
		ds = (struct flash_ioctl_devsize *)buf;

		if (NULL == ds) {
			return (dev->size + 1);
		}

		ds->dev_size = dev->size + 1;

		return ENOERR;

	case FLASH_IOCTL_DEVADDR:
		da = (struct flash_ioctl_devaddr *)buf;

		if (NULL == da) {
			return 0;
		}

		da->dev_addr = 0;

		return ENOERR;

	case FLASH_IOCTL_BLOCKSIZE:
		bs = (struct flash_ioctl_blocksize *)buf;

		if (NULL == bs) {
			return (dev->block_info[0].block_size);
		}

		bs->block_size = dev->block_info[0].block_size;
		return ENOERR;

	default:
		return -EINVAL;
	}
}

/* XXX we have to do it because abstract modules
 * don't support unit init */
#include <embox/unit.h>
EMBOX_UNIT_INIT(_flash_devs_init);
static int _flash_devs_init(void) {
	return flash_devs_init();
}
