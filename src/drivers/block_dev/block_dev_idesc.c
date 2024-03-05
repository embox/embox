/**
 * @file
 * @brief DVFS-specific bdev handling
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-01
 */

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include <drivers/block_dev.h>
#include <drivers/device.h>
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

static ssize_t bdev_idesc_read(struct idesc *desc, const struct iovec *iov,
    int cnt) {
	void *buf;
	size_t nbyte;
	struct file_desc *file;
	struct block_dev *bdev;
	struct dev_module *devmod;
	size_t blk_no;
	int res;
	off_t pos;
	char *cache_buf = NULL;

	assert(iov);
	buf = iov->iov_base;
	assert(cnt == 1);
	nbyte = iov->iov_len;

	file = (struct file_desc *)desc;

	pos = file_get_pos(file);

	devmod = file_get_inode_data(file);
	bdev = dev_module_to_bdev(devmod);
	if (!bdev->parent_bdev) {
		/* It's not a partition */
		blk_no = pos / bdev->block_size;
	}
	else {
		/* It's a partition */
		blk_no = bdev->start_offset + (pos / bdev->block_size);
		bdev = bdev->parent_bdev;
	}

	if (pos >= bdev->size) {
		file_set_pos(file, bdev->size);
		return 0;
	}

	if (cnt != bdev->block_size) {
		cache_buf = malloc(bdev->block_size);
		if (!cache_buf) {
			log_error("could not alloc cache_buff (%zi)", bdev->block_size);
			return -ENOMEM;
		}
	}

	assert(bdev->driver);
	assert(bdev->driver->bdo_read);
	if (cache_buf) {
		res = bdev->driver->bdo_read(bdev, cache_buf, bdev->block_size, blk_no);
		if (res > 0) {
			res = nbyte;
			memcpy(buf, &cache_buf[pos % bdev->block_size], nbyte);
		}
	}
	else {
		res = bdev->driver->bdo_read(bdev, buf, nbyte, blk_no);
	}
	if (res > 0) {
		file_set_pos(file, pos + res);
	}

	if (!cache_buf) {
		free(cache_buf);
	}

	return res;
}

static ssize_t bdev_idesc_write(struct idesc *desc, const struct iovec *iov,
    int cnt) {
	struct file_desc *file;
	struct dev_module *devmod;
	struct block_dev *bdev;
	size_t blk_no;
	int res;
	off_t pos;

	assert(desc);
	assert(iov);
	assert(cnt == 1);

	file = (struct file_desc *)desc;

	pos = file_get_pos(file);

	devmod = file_get_inode_data(file);
	bdev = dev_module_to_bdev(devmod);
	if (!bdev->parent_bdev) {
		/* It's not a partition */
		blk_no = pos / bdev->block_size;
	}
	else {
		/* It's a partition */
		blk_no = bdev->start_offset + (pos / bdev->block_size);
		bdev = bdev->parent_bdev;
	}

	if (pos >= bdev->size) {
		file_set_pos(file, bdev->size);
		return 0;
	}

	assert(bdev->driver);
	assert(bdev->driver->bdo_write);
	res = bdev->driver->bdo_write(bdev, (void *)iov->iov_base, iov->iov_len,
	    blk_no);
	if (res > 0) {
		file_set_pos(file, pos + res);
	}

	return res;
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
