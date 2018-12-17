/**
 * @file
 * @brief
 *
 * @date 03.11.10
 * @author Anton Bondarev
 */

#ifndef DEVICE_H_
#define DEVICE_H_


#define has_file
#include <fs/idesc.h>
struct idesc;
struct file {
	struct idesc f_idesc;
};

#include <drivers/device.h>

#include <util/array.h>
#include <util/macro.h>

#define CHAR_DEV_DEF(cname, file_op, idesc_op, init_func) \
	static const struct dev_operations MACRO_GUARD(dev_ops) = { \
		.open = init_func, \
	}; \
	static const struct device MACRO_GUARD(ch_ops) = { \
		.dev_dops = &MACRO_GUARD(dev_ops), \
		.dev_iops = idesc_op, \
	}; \
	ARRAY_SPREAD_DECLARE(const struct dev_module, __device_registry); \
	ARRAY_SPREAD_ADD(__device_registry, { \
			.name = cname, \
			.device   = (struct device *)&MACRO_GUARD(ch_ops), \
			.fops = file_op })

extern int char_dev_init_all(void);
extern int char_dev_register(const char *name, const struct file_operations *ops,
		const struct dev_module *dev_module);
extern int char_dev_idesc_fstat(struct idesc *idesc, void *buff);

#endif /* DEVICE_H_ */
