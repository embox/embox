/*
 * @file
 *
 * @date 28.11.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <embox/device.h>
#include <fs/file_desc.h>
#include <fs/node.h>
#include <fs/vfs.h>
#include <fs/file_operation.h>
#include <stdlib.h>
#include <util/array.h>

ARRAY_SPREAD_DEF(const device_module_t, __device_registry);

int char_dev_init_all(void) {
	int ret;
	size_t i;

	for (i = 0; i < ARRAY_SPREAD_SIZE(__device_registry); ++i) {
		if (__device_registry[i].init != NULL) {
			ret = __device_registry[i].init();
			if (ret != 0) {
				return ret;
			}
		}
	}

	return 0;
}

int char_dev_register(const char *name, const struct kfile_operations *ops) {
	struct node *nod, *devnod;
	struct nas *dev_nas;

	nod = vfs_find_node("/dev", NULL);
	if (nod == NULL) {
		return -1;
	}

	devnod = vfs_add_path(name, nod);
	if (devnod == NULL) {
		return -1;
	}

	dev_nas = devnod->nas;
	dev_nas->fs = alloc_filesystem("empty");
	if (dev_nas->fs == NULL) {
		return -1;
	}

	dev_nas->fs->file_op = ops;

	return 0;
}
