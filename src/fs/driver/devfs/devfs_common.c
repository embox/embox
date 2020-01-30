/**
 * @file devfs_common.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 30.01.2020
 */
#include <sys/stat.h>

#include <drivers/device.h>
#include <fs/inode.h>
#include <fs/file_desc.h>
#include <fs/file_operation.h>

extern struct idesc_ops idesc_bdev_ops;
static struct idesc *devfs_open(struct inode *node, struct idesc *desc) {
	struct dev_module *dev;

	if (S_ISBLK(node->i_mode)) {
		desc->idesc_ops = &idesc_bdev_ops;
		return desc;
	}

	dev = inode_priv(node);
	assert(dev->dev_open);

	return dev->dev_open(dev, dev->dev_priv);
}

static int devfs_ioctl(struct file_desc *desc, int request, void *data) {
	return 0;
}

int devfs_create(struct inode *i_new, struct inode *i_dir, int mode) {
	return 0;
}

struct file_operations devfs_fops = {
	.open  = devfs_open,
	.ioctl = devfs_ioctl,
};
