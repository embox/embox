/*
 * @file
 *
 * @date 28.11.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */
#include <errno.h>
#include <stdlib.h>

#include <drivers/char_dev.h>
#include <fs/file_desc.h>
#include <fs/node.h>
#include <fs/vfs.h>
#include <fs/file_operation.h>

#include <kernel/printk.h>

#include <util/array.h>
#include <util/err.h>
#include <util/log.h>

ARRAY_SPREAD_DEF(const struct dev_module, __device_registry);

int char_dev_init_all(void) {
	const struct dev_module *dev_module;

	array_spread_foreach_ptr(dev_module, __device_registry) {
		char_dev_register(dev_module->name, dev_module->fops, dev_module);
	}

	return 0;
}

/* This stub is supposed to be used when there's no need
 * for device-specific idesc_ops.fstat() */
int char_dev_idesc_fstat(struct idesc *idesc, void *buff) {
	struct stat *sb;

	assert(buff);
	sb = buff;
	memset(sb, 0, sizeof(struct stat));
	sb->st_mode = S_IFCHR;

	return 0;
}

static struct idesc *char_dev_open(struct node *node, struct file_desc *file_desc, int flags) {
	struct dev_module *dev = node->nas->fi->privdata;
	int ret;

	if (!dev) {
		log_error("Can't open char device");
		return NULL;
	}

	dev->d_idesc = &file_desc->idesc;

	if (dev->device->dev_dops->open != NULL) {
		ret = dev->device->dev_dops->open(dev, NULL);
		if (ret != 0) {
			return err_ptr(ret);
		}
	}

	file_desc->idesc.idesc_ops = dev->device->dev_iops;

	return &file_desc->idesc;
}

static struct file_operations char_file_ops = {
	.open = char_dev_open,
};

int char_dev_register(const char *name, const struct file_operations *ops,
		const struct dev_module *dev_module) {
	struct path  node;
	struct nas *dev_nas;

	if (vfs_lookup("/dev", &node)) {
		return -ENOENT;
	}

	if (node.node == NULL) {
		return -ENODEV;
	}

	vfs_create_child(&node, name, S_IFCHR | S_IRALL | S_IWALL, &node);
	if (!(node.node)) {
		return -1;
	}

	dev_nas = node.node->nas;
	dev_nas->fs = filesystem_create("empty");
	if (dev_nas->fs == NULL) {
		return -ENOMEM;
	}

	dev_nas->fs->file_op = ops ? ops : &char_file_ops;
	node.node->nas->fi->privdata = (void *) dev_module;

	return 0;
}
