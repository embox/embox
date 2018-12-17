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
#include <fs/idesc.h>

#define DEV_NAME_LEN \
	OPTION_MODULE_GET(embox__driver__common, NUMBER, device_name_len)

#define MAX_DEV_MODULE_COUNT \
	OPTION_MODULE_GET(embox__driver__common, NUMBER, max_dev_module_count)

#define STATIC_DEVMOD_ID -1

struct device;
struct dev_module;
struct dev_operations;
struct idesc;
struct file;
struct idesc_ops;

struct device {
	const struct dev_operations *dev_dops;
	const struct idesc_ops *dev_iops;
};

struct dev_operations {
	int  (*open)   (struct dev_module *mod, void *dev_priv);
	int  (*probe)  (struct dev_module *mod, void *dev_priv);
	void (*remove) (struct dev_module *mod);
};

struct dev_module {
	int    dev_id;
	char   name[DEV_NAME_LEN];
	struct device *device;
	struct idesc *d_idesc;
	void  *dev_priv;

	const struct file_operations *fops;
};

extern struct dev_module *dev_module_create(struct device *dev,
	const char *name, void *privdata);

extern int dev_module_destroy(struct dev_module *dev);

#endif /* COMMON_DEVICE_H_ */
