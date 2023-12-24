/**
 * @file
 * @brief Declarations for char devices
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-05
 */

#ifndef CHAR_DEV_DVFS_H_
#define CHAR_DEV_DVFS_H_

#include <drivers/device.h>
#include <framework/mod/options.h>
#include <kernel/task/resource/idesc.h>
#include <util/array.h>

#include <config/embox/driver/char_dev.h>

#define MAX_CDEV_QUANTITY \
	OPTION_MODULE_GET(embox__driver__char_dev, NUMBER, dev_quantity)

#define CHAR_DEV_DEF(cdev_name, device_ops, idesc_ops, priv)               \
	ARRAY_SPREAD_DECLARE(const struct dev_module, __char_device_registry); \
	ARRAY_SPREAD_ADD_NAMED(__char_device_registry,                         \
	    MACRO_CONCAT(__char_device_registry_ptr_, cdev_name),              \
	    {                                                                  \
	        .dev_id = DEVID_CDEV,                                          \
	        .name = MACRO_STRING(cdev_name),                               \
	        .dev_ops = device_ops,                                         \
	        .dev_iops = idesc_ops,                                         \
	        .dev_priv = priv,                                              \
	    })

struct dev_module;

struct idesc_dev {
	struct idesc idesc;
	struct dev_module *dev;
};

extern int char_dev_init_all(void);
extern int char_dev_register(struct dev_module *cdev);
extern int char_dev_unregister(struct dev_module *cdev);
extern int char_dev_idesc_fstat(struct idesc *idesc, void *buff);
extern struct idesc *char_dev_idesc_create(struct dev_module *cdev);

static inline struct dev_module *idesc_to_dev_module(struct idesc *desc) {
	struct idesc_dev *d = (struct idesc_dev *)desc;

	return d->dev;
}

extern struct idesc *char_dev_default_open(struct dev_module *cdev, void *priv);
extern void char_dev_default_close(struct idesc *idesc);

#endif /* CHAR_DEV_DVFS_H_ */
