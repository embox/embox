/**
 * @file
 * @brief Handle char devices
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-05
 */

#include <drivers/char_dev.h>
#include <util/array.h>

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

int char_dev_register(const char *name, struct file_operations *fops) {
#if 0
	char full_path[256];
	struct lookup lu;

	/* Get root of devfs in smarter way? */
	strcpy(full_path, "/dev/");
	strcat(full_path, name);

	dvfs_lookup("/dev", &lu);
	lu.parent = lu.item;
	lu.item = NULL;

	if (!lu.parent)
		return -ENOENT; /* devfs is not mounted */

	if (0 > dvfs_create_new(full_path, &lu, S_IFCHR | S_IRALL | S_IWALL))
		return -1;
#endif
	return 0;
}
