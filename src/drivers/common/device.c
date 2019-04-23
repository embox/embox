/**
 * @file device.c
 * @brief Most common stuff for block and char devices
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-11-03
 */

#include <string.h>

#include <fs/idesc.h>

#include <drivers/device.h>
#include <mem/misc/pool.h>
#include <util/indexator.h>

POOL_DEF(dev_module_pool, struct dev_module, MAX_DEV_MODULE_COUNT);
INDEX_DEF(dev_module_idx, 0, MAX_DEV_MODULE_COUNT);

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
	void 		   (*close) (struct idesc *),
	const struct idesc_ops *dev_iops,
	void *privdata
) {
	struct dev_module *devmod;
	int id;

	assert(dev_iops);
	assert(name);
	assert(privdata); /* No known devices without privdata. Currently
			   *  it's up to legacy of the old FS */

	while (*name == '/') {
		name++;
	}

	devmod = pool_alloc(&dev_module_pool);
	id = index_alloc(&dev_module_idx, INDEX_MIN);
	if (id == INDEX_NONE) {
		pool_free(&dev_module_pool, devmod);
		return NULL;
	}

	memset(devmod, 0, sizeof(*devmod));
	devmod->dev_id = id;
	strncpy(devmod->name, name, DEV_NAME_LEN);
	devmod->name[DEV_NAME_LEN - 1] = '\0';
	devmod->dev_iops = dev_iops;
	devmod->dev_open = open;
	devmod->dev_close = close;
	devmod->dev_priv = privdata;

	return devmod;
}

/**
 * @brief Free system resources related to given device module
 *
 * @param dev Given device module
 *
 * @return Zero or negative error number if failed
 */
int dev_module_destroy(struct dev_module *dev) {
	index_free(&dev_module_idx, dev->dev_id);
	pool_free(&dev_module_pool, dev);
	return 0;
}
