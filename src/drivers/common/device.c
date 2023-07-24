/**
 * @file device.c
 * @brief Most common stuff for block and char devices
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-11-03
 */

#include <string.h>

#include <kernel/task/resource/idesc.h>

#include <drivers/device.h>
#include <mem/misc/pool.h>
#include <util/indexator.h>

POOL_DEF(dev_module_pool, struct dev_module, MAX_DEV_MODULE_COUNT);

/**
 * @brief initialize device module with given parameters
 *
 * @param dev Device driver to which dev module would be realted
 * @param name How device should be introduced in devfs
 * @param privdata Pointer to block_dev, char_dev and so on
 *
 * @return
 */
struct dev_module *dev_module_init(struct dev_module *devmod,
	const char *name,
	struct idesc * (*open)  (struct dev_module *, void *),
	/* void           (*close) (struct idesc *), */
	const struct idesc_ops *dev_iops,
	void *privdata
) {
	assert(devmod);
	assert(dev_iops);
	assert(name);

	memset(devmod, 0, sizeof(*devmod));

	strncpy(devmod->name, name, DEV_NAME_LEN);
	devmod->name[DEV_NAME_LEN - 1] = '\0';
	devmod->dev_iops = dev_iops;
	devmod->dev_open = open;
	/* devmod->dev_close = close; */
	devmod->dev_priv = privdata;

	devfs_notify_new_module(devmod);

	return devmod;
}

/**
 * @brief Alloc and initialize device module with given parameters
 *
 * @param dev Device driver to which dev module would be realted
 * @param name How device should be introduced in devfs
 * @param privdata Pointer to block_dev, char_dev and so on
 *
 * @return
 */
struct dev_module *dev_module_create(
	const char *name,
	struct idesc * (*open)  (struct dev_module *, void *),
	/* void           (*close) (struct idesc *), */
	const struct idesc_ops *dev_iops,
	void *privdata
) {
	struct dev_module *devmod;

	devmod = pool_alloc(&dev_module_pool);
	if (devmod == NULL) {
		return NULL;
	}

	return dev_module_init(devmod, name, open, dev_iops, privdata);
}

/**
 * @brief Free system resources related to given device module
 *
 * @param dev Given device module
 *
 * @return Zero or negative error number if failed
 */
int dev_module_deinit(struct dev_module *dev) {
	devfs_notify_del_module(dev);
	return 0;
}

/**
 * @brief Free system resources related to given device module
 *
 * @param dev Given device module
 *
 * @return Zero or negative error number if failed
 */
int dev_module_destroy(struct dev_module *dev) {
	dev_module_deinit(dev);
	pool_free(&dev_module_pool, dev);
	return 0;
}
