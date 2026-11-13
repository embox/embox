/**
 * @file
 * @brief DVFS-specific bdev handling
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-01
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include <drivers/block_dev.h>
#include <drivers/dev_module.h>
#include <fs/file_desc.h>
#include <util/err.h>
#include <util/log.h>

extern const struct idesc_ops idesc_file_ops;
static void bdev_idesc_close(struct idesc *desc) {
	/* It's assumed that block device may be accesed
	 * via idesc ops only if they were opened from /dev/,
	 * so we need to close bdev idesc as it as a file */
	idesc_file_ops.close(desc);
}

/* The device carrying the driver and the block holding byte 0 of FILE's
 * device. Returns the bytes from POS to the end of the opened device (for a
 * partition its own size, not its disk's). */
static uint64_t bdev_idesc_target(struct file_desc *file, off_t pos,
    struct block_dev **dev, uint64_t *first_blk) {
	struct block_dev *bdev;
	uint64_t size;

	bdev = dev_module_to_bdev(file_get_inode_data(file));
	size = bdev->size;

	*first_blk = 0;
	if (bdev->parent_bdev) {
		*first_blk = bdev->start_offset;
		bdev = bdev->parent_bdev;
	}
	*dev = bdev;

	assert(bdev->driver);

	return ((pos < 0) || ((uint64_t)pos >= size)) ? 0 : size - pos;
}

/* The driver takes whole blocks only (the bdo_read/bdo_write contract); a
 * partial block goes through a bounce buffer, read-modify-write. */
static ssize_t bdev_idesc_xfer(struct idesc *desc, const struct iovec *iov,
    int cnt, int write) {
	struct file_desc *file;
	struct block_dev *bdev;
	uint64_t first_blk, left;
	size_t bs, done, nbyte;
	char *bounce = NULL;
	char *buf;
	off_t pos;
	int res = 0;

	assert(desc);
	assert(iov);
	assert(cnt == 1);

	file = (struct file_desc *)desc;
	pos = file_get_pos(file);

	left = bdev_idesc_target(file, pos, &bdev, &first_blk);
	if (left == 0) {
		return 0;
	}

	bs = bdev->block_size;
	buf = iov->iov_base;
	nbyte = iov->iov_len;
	if (nbyte > left) {
		nbyte = left;
	}

	for (done = 0; done < nbyte; done += (size_t)res) {
		uint64_t at = (uint64_t)pos + done;
		uint64_t blk = first_blk + at / bs;
		size_t off = at % bs;
		size_t chunk = bs - off;

		if (chunk > nbyte - done) {
			chunk = nbyte - done;
		}

		if (off == 0 && chunk == bs) {
			res = write
			    ? bdev->driver->bdo_write(bdev, buf + done, bs, blk)
			    : bdev->driver->bdo_read(bdev, buf + done, bs, blk);
		} else {
			if (!bounce && !(bounce = malloc(bs))) {
				res = -ENOMEM;
				break;
			}
			res = bdev->driver->bdo_read(bdev, bounce, bs, blk);
			if (res == (int)bs && write) {
				memcpy(bounce + off, buf + done, chunk);
				res = bdev->driver->bdo_write(bdev, bounce, bs, blk);
			} else if (res == (int)bs) {
				memcpy(buf + done, bounce + off, chunk);
			}
		}

		if (res != (int)bs) {
			if (res >= 0) {
				res = -EIO;
			}
			break;
		}
		res = (int)chunk;
	}

	free(bounce);

	if (done > 0) {
		file_set_pos(file, pos + done);
		return done;
	}

	return res;
}

static ssize_t bdev_idesc_read(struct idesc *desc, const struct iovec *iov,
    int cnt) {
	return bdev_idesc_xfer(desc, iov, cnt, 0);
}

static ssize_t bdev_idesc_write(struct idesc *desc, const struct iovec *iov,
    int cnt) {
	return bdev_idesc_xfer(desc, iov, cnt, 1);
}

static int bdev_idesc_ioctl(struct idesc *idesc, int cmd, void *args) {
	struct dev_module *devmod;
	struct block_dev *bdev;
	struct file_desc *file;

	assert(idesc);

	file = (struct file_desc *)idesc;

	devmod = file_get_inode_data(file);
	bdev = dev_module_to_bdev(devmod);

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return bdev->size;
	case IOCTL_GETBLKSIZE:
		return bdev->block_size;
	default:
		if (bdev->parent_bdev) {
			bdev = bdev->parent_bdev;
		}
		assert(bdev->driver);
		if (NULL == bdev->driver->bdo_ioctl)
			return -ENOSYS;

		return bdev->driver->bdo_ioctl(bdev, cmd, args, 0);
	}
}

static int bdev_idesc_fstat(struct idesc *idesc, struct stat *stat) {
	assert(stat);

	memset(stat, 0, sizeof(struct stat));
	stat->st_mode = S_IFBLK;

	return 0;
}

struct idesc_ops idesc_bdev_ops = {
    .close = bdev_idesc_close,
    .id_readv = bdev_idesc_read,
    .id_writev = bdev_idesc_write,
    .ioctl = bdev_idesc_ioctl,
    .fstat = bdev_idesc_fstat,
};
