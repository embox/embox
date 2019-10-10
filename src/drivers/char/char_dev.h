/**
 * @file
 * @brief
 *
 * @date 03.11.10
 * @author Anton Bondarev
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include <fs/idesc.h>
#include <fs/file_operation.h>
#include <drivers/device.h>
#include <util/array.h>

#define CHAR_DEV_DEF(chname, open_fn, close_fn, idesc_op, priv) \
	ARRAY_SPREAD_DECLARE(const struct dev_module, __device_registry); \
	ARRAY_SPREAD_ADD(__device_registry, { \
			.name = chname, \
			.dev_priv = priv, \
			.dev_iops = idesc_op, \
			.dev_open = open_fn, \
			.dev_close = close_fn, \
			 })
#define _IDESC_DEV_ /* XXX */
struct idesc_dev {
	struct idesc idesc;
	struct dev_module *dev;
};

extern int char_dev_init_all(void);
extern int char_dev_register(const struct dev_module *cdev);
extern int char_dev_idesc_fstat(struct idesc *idesc, void *buff);
extern int cdev_idesc_alloc(struct dev_module *cdev);
extern struct idesc *char_dev_idesc_create(struct dev_module *cdev);

static inline struct dev_module *idesc_to_dev_module(struct idesc *desc) {
	struct idesc_dev *idev = mcast_out(desc, struct idesc_dev, idesc);

	return idev->dev;
}

extern struct idesc *char_dev_default_open(struct dev_module *cdev, void *priv);
extern void char_dev_default_close(struct idesc *idesc);

#endif /* DEVICE_H_ */
