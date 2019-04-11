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
#include <sys/uio.h>
#include <stddef.h>
#include <util/err.h>

#include <drivers/block_dev.h>
#include <drivers/device.h>
#include <fs/dvfs.h>

static void bdev_idesc_close(struct idesc *desc) {
}

static ssize_t bdev_idesc_read(struct idesc *desc, const struct iovec *iov, int cnt) {
	void *buf;
	size_t nbyte;
	struct file *file;
	struct block_dev *bdev;
	struct dev_module *devmod;
	size_t blk_no;
	int res;

	assert(iov);
	buf = iov->iov_base;
	assert(cnt == 1);
	nbyte = iov->iov_len;

	file = (struct file *) desc;

	devmod = file->f_inode->i_data;
	bdev = devmod->dev_priv;
	if (!bdev->parrent_bdev) {
		/* It's not a partition */
		blk_no = file->pos / bdev->block_size;
	} else {
		/* It's a partition */
		blk_no = bdev->start_offset + (file->pos / bdev->block_size);
		bdev = bdev->parrent_bdev;
	}

	assert(bdev->driver);
	assert(bdev->driver->read);
	res = bdev->driver->read(bdev, buf, nbyte, blk_no);
	if (res < 0) {
		return res;
	}

	file->pos += res;
	return res;
}

static ssize_t bdev_idesc_write(struct idesc *desc, const struct iovec *iov, int cnt) {
	struct file *file;
	struct dev_module *devmod;
	struct block_dev *bdev;
	size_t blk_no;
	int res;

	assert(desc);
	assert(iov);
	assert(cnt == 1);

	file = (struct file *)desc;

	devmod = file->f_inode->i_data;
	bdev = devmod->dev_priv;
	if (!bdev->parrent_bdev) {
		/* It's not a partition */
		blk_no = file->pos / bdev->block_size;
	} else {
		/* It's a partition */
		blk_no = bdev->start_offset + (file->pos / bdev->block_size);
		bdev = bdev->parrent_bdev;
	}

	assert(bdev->driver);
	assert(bdev->driver->write);
	res = bdev->driver->write(bdev, (void *)iov->iov_base, iov->iov_len, blk_no);
	if (res < 0) {
		return res;
	}
	file->pos += res;

	return res;
}

static int bdev_idesc_ioctl(struct idesc *idesc, int cmd, void *args) {
	struct dev_module *devmod;
	struct block_dev *bdev;
	struct file *file;

	assert(idesc);

	file = (struct file *) idesc;
	assert(file->f_inode);
	assert(file->f_inode->i_data);

	devmod = file->f_inode->i_data;
	bdev = devmod->dev_priv;

	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return bdev->size;
	case IOCTL_GETBLKSIZE:
		return bdev->block_size;
	default:
		if (bdev->parrent_bdev) {
			bdev = bdev->parrent_bdev;
		}
		assert(bdev->driver);
		if (NULL == bdev->driver->ioctl)
			return -ENOSYS;

		return bdev->driver->ioctl(bdev, cmd, args, 0);
	}
}

static int bdev_idesc_fstat(struct idesc *idesc, void *buff) {
	struct stat *sb;

	assert(buff);
	sb = buff;
	memset(sb, 0, sizeof(struct stat));
	sb->st_mode = S_IFBLK;

	return 0;
}

struct idesc_ops idesc_bdev_ops = {
	.close = bdev_idesc_close,
	.id_readv  = bdev_idesc_read,
	.id_writev = bdev_idesc_write,
	.ioctl = bdev_idesc_ioctl,
	.fstat = bdev_idesc_fstat
};

/* Need of these functions should be considered, as they don't used anywhere */
#if 0
static struct idesc *bdev_idesc_open(struct inode *node, struct idesc *idesc) {
	/* Assume node belongs to /devfs/ */
	struct dev_module *devmod = node->i_data;
	/*
	devmod->dev_file.f_inode = node;
	devmod->dev_file.pos = 0;
	devmod->dev_file.f_idesc.idesc_ops = &idesc_bdev_ops;

	return &devmod->dev_file.f_idesc;
	*/
	devmod->dev_iops = &idesc_bdev_ops;
	return idesc;
}

struct file_operations bdev_dev_ops = {
	.open = bdev_idesc_open,
};

/* Some useful handlers. All this functions do is converting position
 * into appropriate format for block device */

int bdev_read_block(struct dev_module *devmod, void *buf, int blk) {
	struct block_dev *bdev;

	assert(devmod);
	assert(buf);
	assert(devmod->dev_iops->id_readv);

	bdev = devmod->dev_priv;
	assert(bdev);
	/* TODO check if given devmod is really block device. Probably,
	 * some flags should be added to struct dev_module to check it.
	 * Or maybe pools bounds could be used to check. */

	// devmod->dev_file.pos = blk * bdev->block_size;

	return devmod->dev_iops->id_readv(
			&devmod->dev_file.f_idesc,
			buf,
			bdev->block_size);
}

int bdev_write_block(struct dev_module *devmod, void *buf, int blk) {
	struct block_dev *bdev;

	assert(devmod);
	assert(buf);
	assert(devmod->dev_file.f_idesc.idesc_ops->id_writev);

	bdev = devmod->dev_priv;
	assert(bdev);
	/* TODO check if given devmod is really block device. Probably,
	 * some flags should be added to struct dev_module to check it.
	 * Or maybe pools bounds could be used to check. */

	devmod->dev_file.pos = blk * bdev->block_size;

	return devmod->dev_file.f_idesc.idesc_ops->id_writev(
			&devmod->dev_file.f_idesc,
			buf,
			bdev->block_size);
}

int bdev_write_blocks(struct dev_module *devmod, void *buf, int blk, int count) {
	struct block_dev *bdev;
	int i, err;

	assert(devmod);
	assert(buf);

	bdev = devmod->dev_priv;

	for (i = 0; i < count; i++) {
		err = bdev_write_block(devmod, buf, blk);
		buf += bdev->block_size;
		blk++;

		if (err < 0)
			return err;
	}

	return count * bdev->block_size;;
}

int bdev_read_blocks(struct dev_module *devmod, void *buf, int blk, int count) {
	struct block_dev *bdev;
	int i, err;

	assert(devmod);
	assert(buf);

	bdev = devmod->dev_priv;

	for (i = 0; i < count; i++) {
		err = bdev_read_block(devmod, buf, blk);
		buf += bdev->block_size;
		blk++;

		if (err < 0)
			return err;
	}

	return count * bdev->block_size;;
}
#endif
