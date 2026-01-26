/**
 * @file device.c
 * @brief Most common stuff for block, char and other devices
 * 
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @date 2015-11-03
 */

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include <drivers/device.h>

#include <mem/misc/pool.h>

POOL_DEF(dev_module_pool, struct dev_module, MAX_DEV_MODULE_COUNT);

/**
 * @brief Initialize device module with given parameters
 *
 * @param dev Device driver to which dev module would be realted
 * @param name How device should be introduced in devfs
 * @param privdata Pointer to block_dev, char_dev and so on
 *
 * @return Initialized device module
 */
struct dev_module *dev_module_init(struct dev_module *devmod, const char *name,
    const struct idesc_ops *dev_iops, void *privdata) {
	assert(devmod);
	assert(dev_iops);
	assert(name);

	memset(devmod, 0, sizeof(*devmod));

/* FIXME #pragma GCC diagnostic ignored "-Wstringop-truncation" */
#if defined(__GNUC__) && (__GNUC__ > 7)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif

	strncpy(devmod->name, name, DEV_NAME_LEN);
	devmod->name[DEV_NAME_LEN - 1] = '\0';

#if defined(__GNUC__) && (__GNUC__ > 7)
#pragma GCC diagnostic pop
#endif

	devmod->dev_iops = dev_iops;
	devmod->dev_priv = privdata;

	devfs_notify_new_module(devmod);

	return devmod;
}
#if 0
/* Don't use now */
/**
 * @brief Alloc and initialize device module with given parameters
 *
 * @param dev Device driver to which dev module would be realted
 * @param name How device should be introduced in devfs
 * @param privdata Pointer to block_dev, char_dev and so on
 *
 * @return Allocated device module or NULL if failed
 */
struct dev_module *dev_module_create(const char *name,
    const struct idesc_ops *dev_iops, void *privdata) {
	struct dev_module *devmod;

	devmod = pool_alloc(&dev_module_pool);
	if (devmod == NULL) {
		return NULL;
	}

	return dev_module_init(devmod, name, dev_iops, privdata);
}
#endif

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
