/**
 * @file
 * @brief Declarations for char dev based on dvfs
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-05
 */

#ifndef CHAR_DEV_DVFS_H_
#define CHAR_DEV_DVFS_H_

#include <util/dlist.h>
#include <util/array.h>

struct file_operations;

struct device_module {
	const char *name;
	struct file_operations *fops;
	struct idesc_ops *idesc_ops;
	struct dlist_head cdev_list;
	void *dev_data;
};

#define CHAR_DEV_DEF(name, file_op, idesc_op) \
	ARRAY_SPREAD_DECLARE(const struct device_module, __char_device_registry); \
	ARRAY_SPREAD_ADD(__char_device_registry, {name, file_op, idesc_op} )

extern int char_dev_register(struct device_module *cdev);

extern struct dlist_head cdev_repo_list;

#endif /* CHAR_DEV_DVFS_H_ */
