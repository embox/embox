/**
 * @file
 * @brief Meta-driver declarations
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-29
 */

#ifndef COMMON_DEVICE_H_
#define COMMON_DEVICE_H_

#include <framework/mod/options.h>

#include <config/embox/device/common.h>

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
	char name[DEV_NAME_LEN];

	const struct idesc_ops *dev_iops;

	struct idesc *(*dev_open)(struct dev_module *, void *);
	/* void   (*dev_close) (struct idesc *); */

	void *dev_priv;
};

extern struct dev_module *dev_module_create(const char *name,
    struct idesc *(*open)(struct dev_module *, void *),
    /* void           (*close) (struct idesc *), */
    const struct idesc_ops *dev_iops, void *privdata);

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
    const char *name, struct idesc *(*open)(struct dev_module *, void *),
    /* void           (*close) (struct idesc *), */
    const struct idesc_ops *dev_iops, void *privdata);

extern int dev_module_destroy(struct dev_module *dev);

extern int dev_module_deinit(struct dev_module *dev);

/* Used only in old devfs to create node for new device */
extern void devfs_notify_new_module(struct dev_module *devmod);
extern void devfs_notify_del_module(struct dev_module *devmod);

#endif /* COMMON_DEVICE_H_ */
