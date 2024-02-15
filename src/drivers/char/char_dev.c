/**
 * @file
 * @brief Handle char devices
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-05
 */
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>

#include <drivers/char_dev.h>
#include <drivers/device.h>
#include <lib/libds/array.h>
#include <lib/libds/indexator.h>
#include <util/log.h>

static struct dev_module *devtab[MAX_CDEV_QUANTITY];
INDEX_DEF(char_dev_idx, 0, MAX_CDEV_QUANTITY);

struct dev_module **get_cdev_tab(void) {
	return &devtab[0];
}
extern void devfs_notify_new_module(struct dev_module *devmod);
extern void devfs_notify_del_module(struct dev_module *devmod);

static int char_dev_register_static(struct dev_module *cdev) {
	int cdev_id;

	assert(cdev);

	/* This index is not supposed to be freed */
	cdev_id = index_alloc(&char_dev_idx, INDEX_MIN);
	if (cdev_id == INDEX_NONE) {
		log_error("Failed to register char dev %s", cdev->name);
		return -ENOMEM;
	}

	devtab[cdev_id] = cdev;

	/* XXX Since cdevs were probably allocated as read-only in array spread
	 * we cannot modify its dev_id. Since dev_id is unused currently
	 * it's safely do not assign it at all. */
	/* cdev->dev_id = cdev_id; */

	devfs_notify_new_module(cdev);

	return 0;
}

int char_dev_register(struct dev_module *cdev) {
	int cdev_id;

	assert(cdev);

	/* This index is not supposed to be freed */
	cdev_id = index_alloc(&char_dev_idx, INDEX_MIN);
	if (cdev_id == INDEX_NONE) {
		log_error("Failed to register char dev %s", cdev->name);
		return -ENOMEM;
	}
	cdev->dev_id = DEVID_CDEV | cdev_id;

	devtab[cdev_id] = cdev;

	/* XXX Since cdevs were probably allocated as read-only in array spread
	 * we cannot modify its dev_id. Since dev_id is unused currently
	 * it's safely do not assign it at all. */
	/* cdev->dev_id = cdev_id; */

	devfs_notify_new_module(cdev);

	return 0;
}

int char_dev_unregister(struct dev_module *cdev) {
	assert(cdev);

	if (devtab[cdev->dev_id & DEVID_ID_MASK] == cdev) {
		return -ENODEV;
	}

	devfs_notify_del_module(cdev);

	index_free(&char_dev_idx, cdev->dev_id & DEVID_ID_MASK);

	devtab[cdev->dev_id & DEVID_ID_MASK] = NULL;

	return 0;
}

ARRAY_SPREAD_DEF(const struct dev_module, __char_device_registry);

/* NOTE: this function doesn't actually init anything, it just stores
 * statically allocated char devices to devtab[] */
int char_dev_init_all(void) {
	struct dev_module *cdev;

	array_spread_foreach_ptr(cdev, __char_device_registry) {
		char_dev_register_static(cdev);
	}

	return 0;
}

/* This stub is supposed to be used when there's no need
 * for device-specific idesc_ops.fstat() */
int char_dev_default_fstat(struct idesc *idesc, void *buff) {
	struct stat *sb;

	assert(buff);

	sb = buff;
	memset(sb, 0, sizeof(struct stat));
	sb->st_mode = S_IFCHR;

	return 0;
}

int char_dev_default_open(struct idesc *idesc, void *source) {
	idesc->idesc_priv = source;

	return 0;
}

void char_dev_default_close(struct idesc *idesc) {
}

static const struct idesc_ops char_dev_default_ops = {
    .open = char_dev_default_open,
    .close = char_dev_default_close,
    .fstat = char_dev_default_fstat,
};

const struct idesc_ops *char_dev_get_default_ops(void) {
	return &char_dev_default_ops;
}
