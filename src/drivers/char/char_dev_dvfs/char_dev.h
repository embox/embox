/**
 * @file
 * @brief Declarations for char dev based on dvfs
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-05
 */

#ifndef CHAR_DEV_DVFS_H_
#define CHAR_DEV_DVFS_H_

#include <drivers/device.h>
#include <fs/dvfs.h>
#include <util/array.h>
#include <util/dlist.h>
#include <util/macro.h>
#include <util/member.h>

extern struct file_operations char_dev_fops;

#define CHAR_DEV_DEF(chname, open_fn, idesc_op, priv) \
	static const struct dev_operations MACRO_GUARD(dev_ops) = { \
		.open = open_fn, \
	}; \
	static const struct device MACRO_GUARD(ch_ops) = { \
		.dev_dops = &MACRO_GUARD(dev_ops), \
		.dev_iops = idesc_op, \
	}; \
	ARRAY_SPREAD_DECLARE(const struct dev_module, __char_device_registry); \
	ARRAY_SPREAD_ADD(__char_device_registry, { \
		.dev_file = { \
			.f_ops = &char_dev_fops, \
			.f_idesc = { \
				.idesc_ops = idesc_op, \
			}, \
		}, \
		.dev_id   = STATIC_DEVMOD_ID, \
		.device   = (struct device *)&MACRO_GUARD(ch_ops), \
		.name     = chname, \
		.dev_priv = priv, \
	})

extern int char_dev_register(struct dev_module *cdev);

static inline struct dev_module *idesc_to_dev_module(struct idesc *desc) {
	struct file *f = mcast_out(desc, struct file, f_idesc);

	return mcast_out(f, struct dev_module, dev_file);
}

extern int char_dev_idesc_fstat(struct idesc *idesc, void *buff);

#endif /* CHAR_DEV_DVFS_H_ */
