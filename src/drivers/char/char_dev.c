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

#include <util/array.h>
#include <util/err.h>
#include <util/log.h>
#include <mem/misc/pool.h>

#define IDEV_POOL_SIZE OPTION_GET(NUMBER, cdev_idesc_quantity)
POOL_DEF(idev_pool, struct idesc, IDEV_POOL_SIZE);

ARRAY_SPREAD_DEF(const struct dev_module, __device_registry);

int char_dev_init_all(void) {
	const struct dev_module *cdev;

	array_spread_foreach_ptr(cdev, __device_registry) {
		char_dev_register(cdev);
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

static void char_dev_idesc_close(struct idesc *idesc) {
}

static const struct idesc_ops idesc_char_dev_def_ops = {
		.close = char_dev_idesc_close,
		.fstat = char_dev_idesc_fstat,
};

struct idesc *char_dev_idesc_create(struct dev_module *cdev) {
	struct idesc_dev *idev;

	idev = pool_alloc(&idev_pool);
	if (idev == NULL) {
		log_error("Can't allocate char device");
		return NULL;
	}

	if (cdev) {
		idesc_init(&idev->idesc, cdev->dev_iops, S_IROTH | S_IWOTH);
	} else {
		idesc_init(&idev->idesc, &idesc_char_dev_def_ops, S_IROTH | S_IWOTH);
	}
	idev->dev = cdev;

	return &idev->idesc;
}

struct idesc *char_dev_default_open(struct dev_module *cdev, void *priv) {
	return char_dev_idesc_create(cdev);
}

void char_dev_default_close(struct idesc *idesc) {
	pool_free(&idev_pool, idesc);
}

struct idesc *char_dev_open(struct node *node, int flags) {
	struct dev_module *cdev = node->nas->fi->privdata;

	if (!cdev) {
		log_error("Can't open char device");
		return NULL;
	}

	if (cdev->dev_open != NULL) {
		return cdev->dev_open(cdev, cdev->dev_priv);
	}

	return char_dev_idesc_create(cdev);
}

int char_dev_register(const struct dev_module *cdev) {
	struct path node;
	struct nas *dev_nas;

	if (vfs_lookup("/dev", &node)) {
		return -ENOENT;
	}

	if (node.node == NULL) {
		return -ENODEV;
	}

	vfs_create_child(&node, cdev->name, S_IFCHR | S_IRALL | S_IWALL, &node);
	if (!(node.node)) {
		return -1;
	}

	dev_nas = node.node->nas;
	dev_nas->fs = filesystem_create("devfs");
	if (dev_nas->fs == NULL) {
		return -ENOMEM;
	}

	node.node->nas->fi->privdata = (void *)cdev;

	return 0;
}
