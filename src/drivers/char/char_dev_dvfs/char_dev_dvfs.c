/**
 * @file
 * @brief Handle char devices
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-05
 */
#include <errno.h>
#include <string.h>

#include <util/array.h>
#include <util/dlist.h>

#include <fs/dvfs.h>

#include <drivers/char_dev.h>

DLIST_DEFINE(cdev_repo_list);

ARRAY_SPREAD_DEF(const struct device_module, __char_device_registry);

int char_dev_init_all(void) {
	int ret;
	const struct device_module *dev_module;

	array_spread_foreach_ptr(dev_module, __char_device_registry) {
		if (dev_module->init != NULL) {
			ret = dev_module->init();
			if (ret != 0) {
				return ret;
			}
		}
	}

	return 0;
}

int char_dev_register(struct device_module *cdev) {
	assert(cdev);

	dlist_head_init(&cdev->cdev_list);
	dlist_add_next(&cdev->cdev_list, &cdev_repo_list);

	return 0;
}
