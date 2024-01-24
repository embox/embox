/**
 * @file
 * @brief Handle char devices
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-05
 */
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>

#include <drivers/char_dev.h>
#include <drivers/device.h>
#include <framework/mod/options.h>
#include <mem/misc/pool.h>
#include <lib/libds/array.h>
#include <lib/libds/indexator.h>
#include <util/log.h>

#define IDEV_POOL_SIZE OPTION_GET(NUMBER, cdev_idesc_quantity)
POOL_DEF(idev_pool, struct idesc, IDEV_POOL_SIZE);

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

struct idesc *char_dev_default_open(struct dev_module *cdev, void *priv) {
	return char_dev_idesc_create(cdev);
}

void char_dev_default_close(struct idesc *idesc) {
	pool_free(&idev_pool, idesc);
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
int char_dev_idesc_fstat(struct idesc *idesc, void *buff) {
	struct stat *sb;

	assert(buff);
	sb = buff;
	memset(sb, 0, sizeof(struct stat));
	sb->st_mode = S_IFCHR;

	return 0;
}

static void char_dev_idesc_close(struct idesc *idesc) {
	char_dev_default_close(idesc);
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
		idesc_init(&idev->idesc, cdev->dev_iops, O_RDWR);
	}
	else {
		idesc_init(&idev->idesc, &idesc_char_dev_def_ops, O_RDWR);
	}
	idev->dev = cdev;

	return &idev->idesc;
}
