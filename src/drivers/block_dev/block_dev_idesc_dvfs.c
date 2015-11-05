/**
 * @file
 * @brief DVFS-specific bdev handling
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-01
 */
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <util/err.h>


#include <drivers/block_dev.h>
#include <fs/dvfs.h>

static void bdev_idesc_close(struct idesc *desc) {
	dvfs_destroy_file((struct file *)desc);
}

static ssize_t bdev_idesc_read(struct idesc *desc, void *buf, size_t size) {
	struct file *file;
	struct block_dev *bdev;

	file = (struct file *)desc;

	bdev = file->f_inode->i_data;
	return bdev->driver->read(bdev, buf, size, file->pos / bdev->block_size);
}

static ssize_t bdev_idesc_write(struct idesc *desc, const void *buf, size_t size) {
	struct file *file;
	struct block_dev *bdev;

	file = (struct file *)desc;

	bdev = file->f_inode->i_data;
	assert(bdev->driver);
	assert(bdev->driver->write);

	return bdev->driver->write(bdev, (void *)buf, size, file->pos / bdev->block_size);
}

static int bdev_idesc_ioctl(struct idesc *idesc, int cmd, void *args) {
	struct block_dev *bdev;
	struct file *file;

	assert(idesc);

	file = (struct file *) idesc;
	assert(file->f_inode);
	assert(file->f_inode->i_data);

	bdev = file->f_inode->i_data;

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return bdev->size;
	case IOCTL_GETBLKSIZE:
		return bdev->block_size;
	default:
		assert(bdev->driver);
		if (NULL == bdev->driver->ioctl)
			return -ENOSYS;

		return bdev->driver->ioctl(bdev, cmd, args, 0);
	}
}

struct idesc_ops idesc_bdev_ops = {
	.close = bdev_idesc_close,
	.read  = bdev_idesc_read,
	.write = bdev_idesc_write,
	.ioctl = bdev_idesc_ioctl,
};

static struct idesc *bdev_idesc_open(struct inode *node, struct idesc *idesc) {
	struct file *file;
	file = dvfs_alloc_file();
	if (!file) {
		return err_ptr(ENOMEM);
	}
	*file = (struct file) {
		.f_inode = node,
		.f_idesc  = {
				.idesc_ops   = &idesc_bdev_ops,
		},
	};
	return &file->f_idesc;
}

struct file_operations bdev_dev_ops = {
	.open = bdev_idesc_open,
};
