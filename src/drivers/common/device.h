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

#define DEV_NAME_LEN \
	OPTION_MODULE_GET(embox__driver__common, NUMBER, device_name_len)

struct device;
struct module;
struct dev_operations;
struct idesc_ops;

struct device {
	int dev_id;
	struct dev_operations *dev_dops;
	struct idesc_ops *dev_iops;
};

struct dev_operations {
	int  (*probe)  (struct dev_module *mod);
	void (*remove) (struct dev_module *mod);
};

struct dev_module {
	char name[DEV_NAME_LEN];
	struct device *device;
};

#endif /* COMMON_DEVICE_H_ */
