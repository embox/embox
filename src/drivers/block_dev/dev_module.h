/**
 * @file
 * @brief Meta-driver declarations
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-29
 */

#ifndef DRIVERS_DEV_MODULE_H_
#define DRIVERS_DEV_MODULE_H_

#include <limits.h>

#define DEV_NAME_LEN \
	OPTION_MODULE_GET(embox__device__common, NUMBER, device_name_len)

#define MAX_DEV_MODULE_COUNT \
	OPTION_MODULE_GET(embox__device__common, NUMBER, max_dev_module_count)

#define DEVID_ID_MASK    0x00FF
#define DEVID_FLAGS_MASK 0xFF00
#define DEVID_CDEV       0x0100
#define DEVID_BDEV       0x0200
#define DEVID_STATIC     0x0400

struct idesc;
struct idesc_ops;

struct dev_module {
	int dev_id;
	char name[NAME_MAX];

	const struct idesc_ops *dev_iops;

	void *dev_priv;
};

/**
 * @brief initialize device module with given parameters
 *
 * @param dev Device driver to which dev module would be realted
 * @param name How device should be introduced in devfs
 * @param privdata Pointer to block_dev, char_dev and so on
 *
 * @return
 */
extern struct dev_module *dev_module_init(struct dev_module *devmod,
    const char *name, const struct idesc_ops *dev_iops, void *privdata);

extern int dev_module_deinit(struct dev_module *dev);

/* Used only in old devfs to create node for new device */
extern void devfs_notify_new_module(struct dev_module *devmod);
extern void devfs_notify_del_module(struct dev_module *devmod);

#endif /* DRIVERS_DEV_MODULE_H_ */
