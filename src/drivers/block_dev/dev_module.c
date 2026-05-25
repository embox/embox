/**
 * @file device.c
 * @brief Most common stuff for block, char and other devices
 * 
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @date 2015-11-03
 */

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>

#include <drivers/dev_module.h>

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

	strncpy(devmod->name, name, NAME_MAX);
	devmod->name[NAME_MAX - 1] = '\0';

#if defined(__GNUC__) && (__GNUC__ > 7)
#pragma GCC diagnostic pop
#endif

	devmod->dev_iops = dev_iops;
	devmod->dev_priv = privdata;

	devfs_notify_new_module(devmod);

	return devmod;
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
