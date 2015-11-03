/**
 * @file
 * @brief Meta-driver declarations
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-29
 */

#ifndef COMMON_DEVICE_H_
#define COMMON_DEVICE_H_

#include <config/embox/driver/common.h>
#include <framework/mod/options.h>

#define DEV_NAME_LEN \
	OPTION_MODULE_GET(embox__driver__common, NUMBER, device_name_len)

struct device;
struct dev_module;
struct dev_operations;
struct idesc_ops;

struct device {
	struct dev_operations *dev_dops;
	struct idesc_ops *dev_iops;
};

struct dev_operations {
	int  (*probe)  (struct dev_module *mod, void *dev_priv);
	void (*remove) (struct dev_module *mod);
};

struct dev_module {
	int dev_id;
	char name[DEV_NAME_LEN];
	struct device *device;
	void *dev_priv;
};

extern struct dev_module *dev_module_create(struct device *dev,
	const char *name, void *privdata);

extern int dev_module_destroy(struct dev_module *dev);

#endif /* COMMON_DEVICE_H_ */
